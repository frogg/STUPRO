#include <Utils/TileDownload/ImageDownloader.hpp>
#include <thread>

ImageDownloader::ImageDownloader(OnTileFetched onTileFetched,
                                 QMap<QString, ImageLayerDescription> imageLayers)
	: ImageDownloader(onTileFetched, [](std::exception const & e) {
	KRONOS_LOG_WARN("%s", e.what());
}, imageLayers) { }

ImageDownloader::ImageDownloader(OnTileFetched onTileFetched, OnTileFetchFailed onTileFetchFailed,
                                 QMap<QString, ImageLayerDescription> imageLayers)
	: onTileFetched(onTileFetched), onTileFetchFailed(onTileFetchFailed), imageLayers(imageLayers) { }


void ImageDownloader::fetchTile(int zoomLevel, int tileX, int tileY) {
	this->fetchTile(this->getAvailableLayers(), zoomLevel, tileX, tileY);
}

void ImageDownloader::fetchTile(const QString layer, int zoomLevel, int tileX, int tileY) {
	QList<QString> layers;
	layers.append(layer);
	this->fetchTile(layers, zoomLevel, tileX, tileY);
}

void ImageDownloader::fetchTile(const QList<QString> layers, int zoomLevel, int tileX, int tileY) {
	ImageTileFetcher* fetcher = new ImageTileFetcher(this->getAvailableLayerDescriptions(), layers,
	        zoomLevel, tileX, tileY);

	OnTileFetched onTileFetched = [this, fetcher](ImageTile tile) {
		this->activeFetchers.remove(fetcher);
		this->onTileFetched(tile);
	};
	OnTileFetchFailed onTileFetchFailed = [this, fetcher](std::exception const & e) {
		this->activeFetchers.remove(fetcher);
		this->onTileFetchFailed(e);
	};
	fetcher->setOnTileFetched(onTileFetched);
	fetcher->setOnTileFetchFailed(onTileFetchFailed);

	this->activeFetchers.insert(fetcher);
	std::thread(&ImageTileFetcher::run, fetcher).detach();
}

void ImageDownloader::abortAllDownloads() {
	for (auto fetcher : this->activeFetchers) {
		fetcher->abortFetching();
	}
}

QList<QString> ImageDownloader::getAvailableLayers() const {
	return this->imageLayers.keys();
}

QMap<QString, ImageLayerDescription> ImageDownloader::getAvailableLayerDescriptions() const {
	return this->imageLayers;
}
