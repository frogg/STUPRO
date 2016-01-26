#include <Utils/TileDownload/ImageTileFetcher.hpp>

#include <Utils/TileDownload/ImageCache.hpp>
#include <Utils/TileDownload/ImageDownloadWorker.hpp>

#include <qurl.h>

ImageTileFetcher::ImageTileFetcher(QMap<QString, ImageLayerDescription> availableLayers,
                                   QList<QString> requestedLayers, int zoomLevel, int tileX, int tileY,
                                   ImageDownloader::OnTileFetched onTileFetched) {
	validateLayersAvailable(availableLayers.keys(), requestedLayers);

	for (auto& layerDescription : availableLayers.values()) {
		layerDescription.validateTileLocation(zoomLevel, tileX, tileY);
	}

	this->availableLayers = availableLayers;
	this->requestedLayers = requestedLayers;
	this->zoomLevel = zoomLevel;
	this->tileX = tileX;
	this->tileY = tileY;
	this->onTileFetched = onTileFetched;
}

void ImageTileFetcher::run() {
	// check every image on the requested tile for a cache hit, load it from the cache if possible,
	// otherwise kick of an ImageDownloadWorker
	ImageCache& cache = ImageCache::getInstance();
	QMap<QString, MetaImage> images2;
	std::map<QString, MetaImage> images;
	std::vector<std::unique_ptr<ImageDownloadWorker>> workers;

	for (auto const& layer : this->requestedLayers) {
		if (cache.isImageCached(layer, this->zoomLevel, this->tileX, this->tileY)) {
			// load the image from the cache
			MetaImage image = cache.getCachedImage(layer, this->zoomLevel, this->tileX, this->tileY);
			// images.insert(layer, image);
			images.insert(std::pair<QString, MetaImage>(layer, image));
		} else {
			// kick off an ImageDownloadWorker
			QUrl imageUrl = this->buildTileDownloadUrl(layer);
			int tileSize = this->availableLayers[layer].getTileSize();
			// not-so-nice pointers, but didn't get it to work otherwise :/
			workers.push_back(
			    std::unique_ptr<ImageDownloadWorker>(
			        new ImageDownloadWorker(layer, imageUrl, tileSize, tileSize)
			    )
			);
		}
	}

	for (auto& worker : workers) {
		MetaImage image = worker->getFuture().get();
		images.insert(std::pair<QString, MetaImage>(worker->getLayerName(), image));
		cache.cacheImage(image, worker->getLayerName(), this->zoomLevel, this->tileX, this->tileY);
	}

	ImageTile tile(QMap<QString, MetaImage>(images), this->zoomLevel, this->tileX, this->tileY);
	this->onTileFetched(tile);
}

QUrl ImageTileFetcher::buildTileDownloadUrl(const QString layer) const {
	ImageLayerDescription layerDescription = this->availableLayers[layer];

	return QUrl(layerDescription.buildTileUrl(this->zoomLevel, this->tileX, this->tileY));
}

void ImageTileFetcher::validateLayersAvailable(const QList<QString> availableLayers,
        const QList<QString> requestedLayers) {
	for (QString layer : requestedLayers) {
		if (!availableLayers.contains(layer)) {
			throw InvalidLayerException(layer, availableLayers);
		}
	}
}
