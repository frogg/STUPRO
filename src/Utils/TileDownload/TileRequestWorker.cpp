#include <Utils/TileDownload/TileRequestWorker.hpp>

#include <Utils/TileDownload/ConfigUtil.hpp>
#include <Utils/TileDownload/ImageCache.hpp>

#include <chrono>

TileRequestWorker::TileRequestWorker(QSet<QString> layers,
                                     OnTileFetched onTileFetched,
                                     OnTileFetchFailed onTileFetchFailed)
	: layerConfig(ConfigUtil::loadConfigFile("./res/layers.json")),
	  layers(layers),
	  onTileFetched(onTileFetched),
	  onTileFetchFailed(onTileFetchFailed),
	  shutdownRequested(false) {
	// validate that the requested layers indeed do exist
	for (auto const& layer : layers) {
		if (!this->layerConfig.contains(layer)) {
			throw InvalidLayerException(layer, this->layerConfig.keys());
		}
	}

	this->workerThread = std::thread(&TileRequestWorker::requestLoop, this);
}

TileRequestWorker::~TileRequestWorker() {
	// wait for the worker thread to end
	this->requestShutdown();
	this->workerThread.join();
}

void TileRequestWorker::requestTile(int zoom, int x, int y) {
	ImageLayerDescription::validateTileLocation(zoom, x, y);

	this->requestQueue.append(std::make_shared<TileRequest>(zoom, x, y));
}

void TileRequestWorker::requestAbort() {
	// empty the request queue
	this->requestQueue.clear();
	// post an abort request
	this->requestQueue.append(std::make_shared<AbortRequest>());
}

const QSet<QString> TileRequestWorker::getRequestedLayers() const {
	return this->layers;
}

void TileRequestWorker::requestLoop() {
	while (!this->isShutdownRequested()) {
		// poll for new requests
		if (this->requestQueue.isEmpty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}
		auto request = this->requestQueue.dequeue();

		try {
			request->handle(this);
		} catch (std::exception const& e) {
			this->onTileFetchFailed(e);
		}
	}
}

void TileRequestWorker::handleTileRequest(TileRequest* request) {
	ImageCache& cache = ImageCache::getInstance();
	QSet<QString> missingLayers;

	int zoom = request->zoom;
	int x = request->x;
	int y = request->y;

	ImageTile tile(zoom, x, y);

	// try to fetch all requested images from the cache
	for (auto const& layer : this->layers) {
		bool fetchedFromCache = false;
		if (cache.isImageCached(layer, zoom, x, y)) {
			try {
				MetaImage image = cache.getCachedImage(layer, zoom, x, y);
				tile.getLayers()[layer] = image;

				fetchedFromCache = true;
			} catch (ImageCorruptedException const& e) {
				// since the image is corrupted, delete it
				cache.deleteCachedImage(layer, zoom, x, y);
			}
		}

		if (!fetchedFromCache) {
			missingLayers.insert(layer);
		}
	}

	if (missingLayers.isEmpty()) {
		this->onTileFetched(tile);
	} else {
		// schedule request for all images that couldn't be read from the cache
		this->scheduleJob(WorkerJob(missingLayers, tile));
	}
}

bool TileRequestWorker::isShutdownRequested() {
	return this->shutdownRequested;
}

void TileRequestWorker::requestShutdown() {
	this->shutdownRequested = true;
}
