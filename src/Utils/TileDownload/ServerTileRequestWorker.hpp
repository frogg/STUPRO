#ifndef KRONOS_UTILS_TILE_DOWNLOAD_SERVER_TILE_REQUEST_WORKER_HPP
#define KRONOS_UTILS_TILE_DOWNLOAD_SERVER_TILE_REQUEST_WORKER_HPP

#include <Utils/TileDownload/TileRequestWorker.hpp>

/**
 * TileRequestWorker that doesn't download tiles itself but waits for them to appear on the file
 * system.
 * Only makes sense if the ImageCache directory is a directory shared between multiple paraview
 * instances, with only one of them (the client) responsible for actually downloading the tiles and
 * the other ones (the server) waiting until a requested tile was cached.
 */
class ServerTileRequestWorker : public TileRequestWorker {
	Q_OBJECT;

public:
	/**
	 * Creates a new ServerTileRequestWorker responsible for loading ImageTiles with the given
	 * layers exclusively from the ImageCache.
	 *
	 * @param layers            the ImageTile layers to fetch
	 * @param onTileFetched     callback that will be called when a tile was successfully loaded
	 * @param onTileFetchFailed callback that will be called on fetching failures
	 */
	ServerTileRequestWorker(QSet<QString> layers, TileRequestWorker::OnTileFetched onTileFetched,
	                        TileRequestWorker::OnTileFetchFailed ontileFetchFailed);

	virtual ~ServerTileRequestWorker();

protected:
	/**
	 * Schedules a new job for tiles that were not completely cached yet.
	 *
	 * @param job information on the incomplete tile to fetch
	 */
	virtual void scheduleJob(WorkerJob job);

	/**
	 * Aborts all pending jobs on incomplete tiles.
	 */
	virtual void handleAbortRequest();

private:
	/** Thread for processing cache retrieval requests asynchronously */
	std::thread cacheRetrievalThread;

	/** The queue used to post and process cache retrieval requests */
	QQueue<WorkerJob> pendingCacheRetrievalJobs;

	/**
	 * Starts a loop for processing tile cache retrieval requests synchronously.
	 * Should only be used by the cacheRetrievalThread.
	 */
	void cacheRetrievalLoop();

	/**
	 * Tries to load all missing tiles of the given job from the cache.
	 * If successful, calls the tileFetchedCallback, otherwise re-schedules the job with all tiles
	 * that are still missing for further processing.
	 *
	 * @param job the job to take care of
	 */
	void handleCacheRequest(WorkerJob job);
};

#endif // KRONOS_UTILS_TILE_DOWNLOAD_SERVER_TILE_REQUEST_WORKER_HPP
