#include <Utils/TileDownload/ConfigUtil.hpp>
#include <Utils/TileDownload/ImageDownloader.hpp>
#include <Utils/TileDownload/ImageTileFetcher.hpp>

ImageDownloader::ImageDownloader(OnTileFetched onTileFetched)
	: ImageDownloader(onTileFetched, ConfigUtil::loadConfigFile("./res/layers.json")) { }

ImageDownloader::ImageDownloader(OnTileFetched onTileFetched,
	QMap<QString, ImageLayerDescription> imageLayers)
	: onTileFetched(onTileFetched), imageLayers(imageLayers) { }

void ImageDownloader::fetchTile(int zoomLevel, int tileX, int tileY) {
	this->fetchTile(this->getAvailableLayers(), zoomLevel, tileX, tileY);
}

void ImageDownloader::fetchTile(const QString layer, int zoomLevel, int tileX, int tileY) {
	QList<QString> layers;
	layers.append(layer);
	this->fetchTile(layers, zoomLevel, tileX, tileY);
}

void ImageDownloader::fetchTile(const QList<QString> layers, int zoomLevel, int tileX, int tileY) {
	ImageTileFetcher *fetcher = new ImageTileFetcher(this->getAvailableLayerDescriptions(), layers,
			zoomLevel, tileX, tileY, this->onTileFetched);
	this->fetchThreadPool.start(fetcher);
}

QList<QString> ImageDownloader::getAvailableLayers() const {
	return this->imageLayers.keys();
}

QMap<QString, ImageLayerDescription> ImageDownloader::getAvailableLayerDescriptions() const {
	return this->imageLayers;
}
