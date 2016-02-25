#include <Utils/TileDownload/ServerTileRequestWorker.hpp>
#include <Utils/TileDownload/ImageCache.hpp>

ServerTileRequestWorker::ServerTileRequestWorker(QSet<QString> layers,
                                                OnTileFetched onTileFetched,
                                                OnTileFetchFailed onTileFetchFailed)
        : TileRequestWorker(layers, onTileFetched, onTileFetchFailed) {
    this->cacheRetrievalThread = std::thread(&ServerTileRequestWorker::cacheRetrievalLoop, this);
}

ServerTileRequestWorker::~ServerTileRequestWorker() {
    this->requestShutdown();
    this->cacheRetrievalThread.join();
}

void ServerTileRequestWorker::scheduleJob(WorkerJob job) {
    this->pendingCacheRetrievalJobs.enqueue(job);
}

void ServerTileRequestWorker::handleAbortRequest() {
    // drop all pending jobs
    this->pendingCacheRetrievalJobs.clear();
}

void ServerTileRequestWorker::cacheRetrievalLoop() {
    while (!this->isShutdownRequested()) {
        // check if missing images now appeared in the cache
        if (this->pendingCacheRetrievalJobs.isEmpty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        WorkerJob job = this->pendingCacheRetrievalJobs.dequeue();
        this->handleCacheRequest(job);
    }
}

void ServerTileRequestWorker::handleCacheRequest(WorkerJob job) {
    ImageCache& cache = ImageCache::getInstance();

    int zoom = job.incompleteTile.getZoomLevel();
    int x = job.incompleteTile.getTileX();
    int y = job.incompleteTile.getTileY();

    // check if any missing requested layer was cached by now
    QMutableSetIterator<QString> it(job.missingLayers);
    while (it.hasNext()) {
        QString layer = it.next();
        if (cache.isImageCached(layer, zoom, x, y)) {
            try {
                MetaImage image = cache.getCachedImage(layer, zoom, x, y);
                job.incompleteTile.getLayers()[layer] = image;

                // delete the layer from the missing layers set
                it.remove();
            } catch (ImageCorruptedException const& e) {
                this->onTileFetchFailed(e);
            }
        }
    }

    if (job.missingLayers.isEmpty()) {
        this->onTileFetched(job.incompleteTile);
    } else {
        this->pendingCacheRetrievalJobs.enqueue(job);
    }
}
