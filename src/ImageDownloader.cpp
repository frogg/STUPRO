#include "../include/ImageDownloader.hpp"
#include "../include/ConfigUtil.hpp"

#include <map>
#include <thread>
#include <mutex>
#include <QImage>

#include <iostream>

QNetworkAccessManager ImageDownloader::networkManager;

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

void ImageDownloader::getTile(int zoomLevel, int tileX, int tileY) {
	this->getTile(this->getAvailableLayers(), zoomLevel, tileX, tileY);
}

void ImageDownloader::getTile(QString &layer, int zoomLevel, int tileX, int tileY) {
	QList<QString> layers;
	layers.append(layer);
	this->getTile(layers, zoomLevel, tileX, tileY);
}

void ImageDownloader::getTile(QList<QString> layers, int zoomLevel, int tileX, int tileY) {
	validateTileLocation(zoomLevel, tileX, tileY);

	// TODO: check cache hit, if so, get image from cache, otherwise download it
	// (this->downloadImage) and store it in the cache

	// create a thread that kicks off all image requests, waits until all are completed and
	// calls the tileFetchedCb with the results
	std::thread fetchThread([ = ]() {
		std::map<QString, std::future<MetaImage>> fetchFutures;

		for (QString layerName : layers) {
			// TODO: wait until the ImageCache is implemented
			// if (ImageCache::isImageCached(layerName, zoomLevel, tileX, tileY)) {
			//  QImage tile = ImageCache::getCachedImage(layerName, zoomLevel, tileX, tileY);
			//  fetchedTiles.insert(layerName, tile);
			// } else {

			// trigger the download of the image
			QUrl imageUrl = this->buildTileDownloadUrl(layerName, zoomLevel, tileX, tileY);
			fetchFutures[layerName] = this->downloadImage(imageUrl);
			// }
		}

		QMap<QString, MetaImage> fetchedTiles;
		for (auto iterator = fetchFutures.begin(); iterator != fetchFutures.end(); ++iterator) {
			iterator->second.wait();
		}

		ImageTile tile(fetchedTiles, zoomLevel, tileX, tileY);
		this->tileFetchedCb(tile);
	});
	// detach the thread to keep it running until all downloads are completed
	fetchThread.detach();
}

QList<QString> ImageDownloader::getAvailableLayers() {
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

QUrl ImageDownloader::buildTileDownloadUrl(QString layer, int zoomLevel, int tileX, int tileY) {
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

std::future<MetaImage> ImageDownloader::downloadImage(QUrl &imageUrl) {
	std::promise<MetaImage> imageDownloadedPromise;

	// TODO: actually download the image
	// this shouldn't be a thread later on when actually using QNetworkAccessManager to download the image
	std::thread([&imageDownloadedPromise, imageUrl]() {
		// TODO: check content type and load the image accordingly, either through QImage or convert it from bil16
		QImage dummyImage(512, 512, QImage::Format_RGB32);
		MetaImage dummyMetaImage(dummyImage, 0, 0);
		imageDownloadedPromise.set_value_at_thread_exit(dummyMetaImage);
	}).detach();

	return imageDownloadedPromise.get_future();
}
