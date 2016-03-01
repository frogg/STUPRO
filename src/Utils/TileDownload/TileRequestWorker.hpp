#ifndef KRONOS_UTILS_TILE_DOWNLOAD_TILE_REQUEST_WORKER_HPP
#define KRONOS_UTILS_TILE_DOWNLOAD_TILE_REQUEST_WORKER_HPP

#include <Utils/TileDownload/ImageLayerDescription.hpp>
#include <Utils/TileDownload/ImageTile.hpp>
#include <Utils/TileDownload/TileRequest.hpp>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QSet>
#include <QQueue>

#include <thread>

/**
 * Represents a job to load an image tile that was not yet cached.
 */
struct WorkerJob {
	/** the layers missing from the tile */
	QSet<QString> missingLayers;
	/** the currently incomplete tile */
	ImageTile incompleteTile;

	WorkerJob() { }

	/**
	 * Creates a job with the given parameters.
	 *
	 * @param missingLayers  the layers still missing from the tile
	 * @param incompleteTile the tile with all images and information that could be loaded
	 */
	WorkerJob(QSet<QString> missingLayers, ImageTile incompleteTile)
		: missingLayers(missingLayers), incompleteTile(incompleteTile) { }
};


/**
 * Class responsible for loading ImageTiles from the cache.
 * If the cache was missed, the request will be redirected to the implementation provided by the
 * respective subclass.
 */
class TileRequestWorker : public QObject {
	/*
	 * Instances of these classes will be put in the requestQueue and will later call one of the
	 * handleAbortRequest or handleTileRequest methods provided by this class.
	 */

	friend class TileRequest;
	friend class AbortRequest;

	Q_OBJECT;

public:
	/**
	 * Callback used to notify about fetched tiles.
	 */
	typedef std::function<void(ImageTile)> OnTileFetched;

	/**
	 * Callback used to notify about errors that occurred trying to fetch tiles.
	 */
	typedef std::function<void(std::exception)> OnTileFetchFailed;

	/**
	 * Initializes a TileRequestWorker responsible to fetch ImageTiles with the given layers.
	 *
	 * @param layers            the ImageTile layers to fetch
	 * @param onTileFetched     callback that will be called when a tile was successfully loaded
	 * @param onTileFetchFailed callback that will be called on fetching failures
	 * @param configFile        path to the JSON file from which to read the layer config
	 */
	TileRequestWorker(QSet<QString> layers, OnTileFetched onTileFetched,
	                  OnTileFetchFailed ontileFetchFailed, QString configFile = "./res/layers.json");

	virtual ~TileRequestWorker();

	/**
	 * Posts a request to fetch the tile at the given location.
	 *
	 * @param zoom the zoom level of the tile
	 * @param x    the x location of the tile
	 * @param y    the y location of the tile
	 */
	void requestTile(int zoom, int x, int y);

	/**
	 * Clears the request queue and posts a request to abort all pending tile fetching efforts.
	 */
	void requestAbort();

	const QSet<QString> getRequestedLayers() const;

protected:
	/** Map containing information on available layers */
	QMap<QString, ImageLayerDescription> layerConfig;

	/** The layers to load for each request. */
	QSet<QString> layers;

	/** The queue used to post and process tile requests */
	QQueue<std::shared_ptr<WorkerRequest>> requestQueue;

	/** The callback to call when a tile was fetched */
	OnTileFetched onTileFetched;
	/** The callback to call when the effort to fetch a tile failed unexpectedly */
	OnTileFetchFailed onTileFetchFailed;

	/**
	 * Schedules a new job for tiles that were not completely cached yet.
	 *
	 * @param job information on the incomplete tile to fetch
	 */
	virtual void scheduleJob(WorkerJob job) = 0;

	/**
	 * Aborts all pending jobs on incomplete tiles.
	 */
	virtual void handleAbortRequest() = 0;

	/**
	 * Returns true if all request processing loops should seize operation, leading to the
	 * corresponding threads to exit.
	 *
	 * @returns true if the TileRequestWorker is about to be destroyed and is waiting for worker
	 *          threads to exit
	 */
	bool isShutdownRequested();

	/**
	 * Requests all worker thread to seize processing events and exit.
	 * Should only be called from destructors, since there is no way to restart the worker threads.
	 */
	void requestShutdown();

private:
	/** Thread for processing tile requests asynchronously */
	std::thread workerThread;

	/** Indicates if worker threads should seize operation. */
	bool shutdownRequested;

	/**
	 * Starts a loop for processing tile requests synchronously.
	 * Should only be used by the workerThread.
	 */
	void requestLoop();

	/**
	 * Handles basic tile requests.
	 * If the request could not be satisfied due to the tile not being cached completely, a
	 * corresponding WorkerJob will be scheduled.
	 *
	 * @param request information on the requested tile
	 */
	void handleTileRequest(TileRequest* request);
};

#endif // KRONOS_UTILS_TILE_DOWNLOAD_TILE_REQUEST_WORKER_HPP
