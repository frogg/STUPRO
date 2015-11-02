#include "../include/ImageDownloader.hpp"
#include "../include/ConfigUtil.hpp"

#include <climits>
#include <map>
#include <thread>
#include <mutex>
#include <QEventLoop>
#include <QImage>
#include <QRegExp>

#include <iostream>

ImageDownloader::ImageDownloader(TileFetchedCb tileFetchedCb) {
	this->tileFetchedCb = tileFetchedCb;
	this->imageLayers = ConfigUtil::loadConfigFile("./src/res/layers.cfg");
}

ImageDownloader::ImageDownloader(TileFetchedCb tileFetchedCb,
								 QMap<QString, ImageLayerDescription> imageLayers) {
	this->tileFetchedCb = tileFetchedCb;
	this->imageLayers = imageLayers;
}

ImageDownloader::~ImageDownloader() { }

void ImageDownloader::getTile(int zoomLevel, int tileX, int tileY) const {
	this->getTile(this->getAvailableLayers(), zoomLevel, tileX, tileY);
}

void ImageDownloader::getTile(const QString &layer, int zoomLevel, int tileX, int tileY) const {
	QList<QString> layers;
	layers.append(layer);
	this->getTile(layers, zoomLevel, tileX, tileY);
}

void ImageDownloader::getTile(const QList<QString> layers, int zoomLevel,
							  int tileX, int tileY) const {
	validateTileLocation(zoomLevel, tileX, tileY);
	validateLayersAvailable(layers);

	std::promise<void> layersProcessed;

	// create a thread that kicks off all image requests, waits until all of them are completed and
	// calls the tileFetchedCb with the resulting ImageTile
	std::thread fetchThread([ =, &layers, &layersProcessed]() {
		std::map<QString, std::future<MetaImage>> fetchFutures;
		QMap<QString, MetaImage> fetchedTiles;
		ImageCache *imageCache = &ImageCache::getInstance();

		for (QString layerName : layers) {
			if (imageCache->isImageCached(layerName, zoomLevel, tileX, tileY)) {
				// load the image from the cache
				MetaImage tile = imageCache->getCachedImage(layerName, zoomLevel, tileX, tileY);
				fetchedTiles.insert(layerName, tile);
			} else {
				// trigger the download of the image
				QUrl imageUrl = this->buildTileDownloadUrl(layerName, zoomLevel, tileX, tileY);
				int tileSize = this->imageLayers[layerName].tileSize;
				fetchFutures[layerName] = this->downloadImage(imageUrl, tileSize, tileSize);
			}
		}

		// notify the parent thread that we're done with the layers list and it is safe to let the
		// list go out of scope
		layersProcessed.set_value();

		// go through all futures of tiles to be downloaded
		for (auto it = fetchFutures.begin(); it != fetchFutures.end(); ++it) {
			// extract key and value from the map's iterator
			std::future<MetaImage> *imageDownloadedFuture = &it->second;
			QString layerName = it->first;

			// wait until the image is downloaded
			MetaImage downloadedImage = imageDownloadedFuture->get();

			// cache the downloaded image
			imageCache->cacheImage(downloadedImage, layerName, zoomLevel, tileX, tileY);

			// add the image to the fetched images
			fetchedTiles.insert(layerName, downloadedImage);
		}

		ImageTile tile(fetchedTiles, zoomLevel, tileX, tileY);
		this->tileFetchedCb(tile);
	});

	layersProcessed.get_future().wait();

	// detach the thread to keep it running until all downloads are completed
	fetchThread.detach();
}

QList<QString> ImageDownloader::getAvailableLayers() const {
	return this->imageLayers.keys();
}

void ImageDownloader::validateTileLocation(int zoomLevel, int tileX, int tileY) {
	// check zoom level range
	if (zoomLevel < MIN_ZOOM_LEVEL || zoomLevel > MAX_ZOOM_LEVEL) {
		throw InvalidTileZoomException(zoomLevel, 0, 15);
	}

	// check tile position range
	int maxX = WIDTH_AT_MIN_ZOOM << (zoomLevel - MIN_ZOOM_LEVEL) - 1;
	int maxY = HEIGHT_AT_MIN_ZOOM << (zoomLevel - MIN_ZOOM_LEVEL) - 1;
	if (tileX < 0 || tileX > maxX || tileY < 0 || tileY > maxY) {
		throw InvalidTilePositionException(zoomLevel, 0, maxX, tileX, 0, maxY, tileY);
	}
}

void ImageDownloader::validateLayersAvailable(const QList<QString> &layers) const {
	QList<QString> availableLayers = this->getAvailableLayers();
	for (QString layer : layers) {
		if (!availableLayers.contains(layer)) {
			throw InvalidLayerException(layer, availableLayers);
		}
	}
}

QString ImageDownloader::calculateBoundingBox(int zoomLevel, int tileX, int tileY) {
	float horizontalTilesAtZoomLevel = (float)(WIDTH_AT_MIN_ZOOM << (zoomLevel - MIN_ZOOM_LEVEL));
	float verticalTilesAtZoomLevel = (float)(HEIGHT_AT_MIN_ZOOM << (zoomLevel - MIN_ZOOM_LEVEL));

	float tileWidthAtZoomLevel = 360.0 / horizontalTilesAtZoomLevel;
	float tileHeightAtZoomLevel = 180.0 / verticalTilesAtZoomLevel;

	float latMin = (tileHeightAtZoomLevel * tileY) - 90.0;
	float latMax = (tileHeightAtZoomLevel * (tileY + 1)) - 90.0;
	float longMin = (tileWidthAtZoomLevel * tileX) - 180.0;
	float longMax = (tileWidthAtZoomLevel * (tileX + 1)) - 180.0;

	QString ret("%1,%3,%2,%4");
	ret = ret.arg(QString::number(latMin), QString::number(latMax));
	ret = ret.arg(QString::number(longMin), QString::number(longMax));

	return ret;
}

QUrl ImageDownloader::buildTileDownloadUrl(const QString &layer, int zoomLevel, int tileX,
		int tileY) const {
	// check if the given location is valid, throw an exception otherwise
	validateTileLocation(zoomLevel, tileX, tileY);

	ImageLayerDescription layerDescription = this->imageLayers[layer];
	QString urlString = layerDescription.baseUrl;

	// add format parameters
	urlString += "&format=" + layerDescription.format;

	// add tile size parameters
	urlString += "&width=" + layerDescription.tileSize;
	urlString += "&height=" + layerDescription.tileSize;

	// add bounding box parameters
	urlString += "&bbox=" + calculateBoundingBox(zoomLevel, tileX, tileY);

	return QUrl(urlString);
}

std::future<MetaImage> ImageDownloader::downloadImage(const QUrl &imageUrl, int width,
		int height) const {
	std::promise<std::future<MetaImage>> imageDownloadedFuturePromise;

	std::thread([this, &imageDownloadedFuturePromise, imageUrl, width, height]() {
		std::promise<MetaImage> imageDownloadedPromise;
		imageDownloadedFuturePromise.set_value(imageDownloadedPromise.get_future());
		try {
			QNetworkRequest request(imageUrl);
			QNetworkAccessManager networkManager;
			QNetworkReply *reply = networkManager.get(request);

			QEventLoop replyLoop;
			QObject::connect(reply, SIGNAL(finished()), &replyLoop, SLOT(quit()));

			// wait until the download is finished
			replyLoop.exec();

			// throw an exception if there was a network error
			if (reply->error()) {
				throw ConnectionFailedException(imageUrl, reply->error());
			}

			// check if the request returned a 200 OK status code
			if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
				throw BadStatusCodeException(reply);
			}

			// TODO: check content type and load the image accordingly, either through QImage or convert it from bil16
			QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
			MetaImage metaImage;
			if (QRegExp("^image\\/(png|tiff|jpeg|gif)$").exactMatch(contentType)) {
				QByteArray rawData = reply->readAll();

				QImage image = QImage::fromData(rawData);

				// ensure that the image was read
				if (image.isNull()) {
					throw ImageDecodingFailedException(imageUrl);
				}

				metaImage = MetaImage(image);
			} else if (QRegExp("^application\\/bil16$").exactMatch(contentType)) {
				QByteArray rawData = reply->readAll();

				metaImage = ImageDownloader::decodeBil16(rawData, width, height);
			} else {
				throw UnknownContentTypeException(contentType);
			}

			imageDownloadedPromise.set_value_at_thread_exit(metaImage);
		} catch (...) {
			imageDownloadedFuturePromise.set_exception(std::current_exception());
		}
	}).detach();

	return imageDownloadedFuturePromise.get_future().get();
}

MetaImage ImageDownloader::decodeBil16(const QByteArray &rawData, int width, int height) {
	if (!rawData.size() == width * height * 2) {
		QString errorMessage("Expected raw data length of %1b, but %2b were given");
		throw Bil16DecodingFailedException(errorMessage.arg(width * height * 2, rawData.size()));
	}
	short minHeight = SHRT_MAX;
	short maxHeight = SHRT_MIN;

	// find min and max values used to normalize the image in the second pass
	int rawIndex = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			short heightValue = (rawData[rawIndex] << 8) | (unsigned char)rawData[rawIndex + 1];
			if (heightValue < minHeight) {
				minHeight = heightValue;
			}
			if (heightValue > maxHeight) {
				maxHeight = heightValue;
			}
			rawIndex += 2;
		}
	}

	// use min and max heights to normalize the data and save it in a QImage
	// color values of 0xff correspond to maxHeight, 0x00 means minHeight
	QImage image(width, height, QImage::Format_ARGB32);
	rawIndex = 0;
	float normalizationFactor = (maxHeight - minHeight) / 255.0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			short heightValue = (rawData[rawIndex] << 8) | (unsigned char)rawData[rawIndex + 1];
			short normalizedHeight = (heightValue - minHeight) * normalizationFactor;

			QRgb color = qRgba(normalizedHeight, normalizedHeight, normalizedHeight, normalizedHeight);
			image.setPixel(x, y, color);

			rawIndex += 2;
		}
	}

	return MetaImage(image, minHeight, maxHeight);
}
