#include "../include/ImageDownloader.hpp"
#include "../include/ConfigUtil.hpp"

#include <QImage>
#include <thread>

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
	// TODO: check cache hit, if so, get image from cache, otherwise download it
	// (this->downloadImage) and store it in the cache

	// create a thread that kicks off all image requests, waits until all are completed and
	// calls the tileFetchedCb with the results
	std::thread fetchThread([ = ]() {
		// build up a queue
		QMap<QString, MetaImage *> fetchedTiles;

		foreach (QString layerName, layers) {
			// if (ImageCache::isImageCached(layerName, zoomLevel, tileX, tileY)) {
			//  QImage tile = ImageCache::getCachedImage(layerName, zoomLevel, tileX, tileY);
			//  fetchedTiles.insert(layerName, tile);
			// } else {
			//  // trigger the download of the image
			// }
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

}

QString ImageDownloader::calculateBoundingBox(int zoomLevel, int tileX, int tileY) {
	return QString("");
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

void ImageDownloader::downloadImage(QUrl &imageUrl, ImageDownloadedCb imageDownloadedCb) {
	QImage dummyImage(512, 512, QImage::Format_RGB32);
	MetaImage dummyMetaImage(dummyImage, 0, 0);
	// TODO: actually download the image
	imageDownloadedCb(dummyImage);
}
