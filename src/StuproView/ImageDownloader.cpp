#include "ImageDownloader.hpp"

#include "ConfigUtil.hpp"
#include "ImageTileFetcher.hpp"

ImageDownloader::ImageDownloader(TileFetchedCb tileFetchedCb) {
	this->tileFetchedCb = tileFetchedCb;
	this->imageLayers = ConfigUtil::loadConfigFile("./res/layers.cfg");
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

void ImageDownloader::getTile(const QString layer, int zoomLevel, int tileX, int tileY) {
	QList<QString> layers;
	layers.append(layer);
	this->getTile(layers, zoomLevel, tileX, tileY);
}

void ImageDownloader::getTile(const QList<QString> layers, int zoomLevel,
							  int tileX, int tileY) {
	ImageTileFetcher *fetcher = new ImageTileFetcher(this->getAvailableLayerDescriptions(), layers,
			zoomLevel, tileX, tileY, this->tileFetchedCb);
	this->fetchThreadPool.start(fetcher);
}

QList<QString> ImageDownloader::getAvailableLayers() const {
	return this->imageLayers.keys();
}

QMap<QString, ImageLayerDescription> ImageDownloader::getAvailableLayerDescriptions() const {
	return this->imageLayers;
}
