#include "../include/ImageDownloader.hpp"
#include "../include/ConfigUtil.hpp"

#include <QQueue>
#include <thread>

QNetworkAccessManager ImageDownloader::networkManager;
ImageCache ImageDownloader::imageCache;

ImageDownloader::ImageDownloader(tileFetchedCbType tileFetchedCb) {
	this->tileFetchedCb = tileFetchedCb;
	this->imageLayers = ConfigUtil::loadConfigFile("./src/res/config.cfg");
}

ImageDownloader::ImageDownloader(tileFetchedCbType tileFetchedCb,
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
	std::thread fetchThread([&]() {
		// build up a queue
		QQueue<QString> layerFetchQueue;
		foreach (QString layer, layers) {
			layerFetchQueue.enqueue(layer);
		}

		QMap<QString, QImage> layers;
		while (!layerFetchQueue.isEmpty()) {

		}
		ImageTile tile(layers, zoomLevel, tileX, tileY);
		this->tileFetchedCb(tile);
	});
	fetchThread.detach();
}

QList<QString> ImageDownloader::getAvailableLayers() {
	return this->imageLayers.keys();
}

QString ImageDownloader::buildTileDownloadUrl(QString layer, int zoomLevel, int tileX, int tileY) {
	//
}

QImage ImageDownloader::downloadImageSync(QUrl &imageUrl) {
	QImage dummyImage(512, 512, QImage::Format_RGB32);
	// TODO: actually download the image
	return dummyImage;
}
