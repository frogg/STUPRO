#include <Utils/TileDownload/ImageTileFetcher.hpp>

#include <Utils/TileDownload/ImageCache.hpp>
#include <Utils/TileDownload/ImageDownloadWorker.hpp>

#include <qurl.h>

ImageTileFetcher::ImageTileFetcher(QMap<QString, ImageLayerDescription> availableLayers,
                                   QList<QString> requestedLayers, int zoomLevel, int tileX, int tileY)
	: availableLayers(availableLayers), requestedLayers(requestedLayers), zoomLevel(zoomLevel),
	  tileX(tileX), tileY(tileY), abortRequested(false) {

	// make sure all requested layers are actually valid
	validateLayersAvailable(availableLayers.keys(), requestedLayers);

	// ensure the requested tile position is valid for all requested layers
	for (QString& layer : requestedLayers) {
		availableLayers[layer].validateTileLocation(zoomLevel, tileX, tileY);
	}
}

void ImageTileFetcher::abortFetching() {
	this->abortRequested = true;
}

void ImageTileFetcher::run() {
	// check every image on the requested tile for a cache hit, load it from the cache if possible,
	// otherwise kick off an ImageDownloadWorker
	ImageCache& cache = ImageCache::getInstance();
	QMap<QString, MetaImage> images2;
	std::map<QString, MetaImage> images;
	std::vector<std::unique_ptr<ImageDownloadWorker>> workers;

	for (auto const& layer : this->requestedLayers) {
		if (this->abortRequested) {
			break;
		}

		bool fetchedFromCache = false;
		if (cache.isImageCached(layer, this->zoomLevel, this->tileX, this->tileY)) {
			// load the image from the cache
			try {
				MetaImage image = cache.getCachedImage(layer, this->zoomLevel, this->tileX, this->tileY);
				images.insert(std::pair<QString, MetaImage>(layer, image));

				fetchedFromCache = true;
			} catch (ImageCorruptedException const& e) {
				// if the image is corrupted, delete it
				cache.deleteCachedImage(layer, this->zoomLevel, this->tileX, this->tileY);
			}
		}

		if (!fetchedFromCache) {
			// kick off an ImageDownloadWorker
			QUrl imageUrl = this->buildTileDownloadUrl(layer);
			int tileSize = this->availableLayers[layer].getTileSize();

			workers.push_back(
			    std::unique_ptr<ImageDownloadWorker>(
			        new ImageDownloadWorker(layer, imageUrl, tileSize, tileSize)
			    )
			);
		}
	}

	try {
		for (auto& worker : workers) {
			if (this->abortRequested) {
				throw DownloadAbortedException();
			}

			auto workerFuture = worker->getFuture();
			std::future_status workerStatus = std::future_status::timeout;

			// wait for the download to complete or to be aborted
			while (workerStatus != std::future_status::ready) {
				workerStatus = workerFuture.wait_for(std::chrono::microseconds(10));
			}

			// if the download wasn't aborted, extract the image from the worker and cache it
			MetaImage image = workerFuture.get();
			images.insert(std::pair<QString, MetaImage>(worker->getLayerName(), image));
			cache.cacheImage(image, worker->getLayerName(), this->zoomLevel, this->tileX, this->tileY);
		}

		if (this->abortRequested) {
			throw DownloadAbortedException();
		}

		ImageTile tile(QMap<QString, MetaImage>(images), this->zoomLevel, this->tileX, this->tileY);
		this->onTileFetched(tile);
	} catch (std::exception const& e) {
		this->onTileFetchFailed(e);
	}
}

void ImageTileFetcher::setOnTileFetched(OnTileFetched onTileFetched) {
	this->onTileFetched = onTileFetched;
}

void ImageTileFetcher::setOnTileFetchFailed(OnTileFetchFailed onTileFetchFailed) {
	this->onTileFetchFailed = onTileFetchFailed;
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
