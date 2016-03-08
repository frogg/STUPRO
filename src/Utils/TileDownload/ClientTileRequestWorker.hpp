#ifndef KRONOS_UTILS_TILE_DOWNLOAD_CLIENT_TILE_REQUEST_WORKER_HPP
#define KRONOS_UTILS_TILE_DOWNLOAD_CLIENT_TILE_REQUEST_WORKER_HPP

#include <Utils/TileDownload/TileRequestWorker.hpp>

#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>


struct ImageDownloadJob : public WorkerJob {
	bool aborted = false;
	QSet<QNetworkReply*> pendingReplies;

	ImageDownloadJob() = default;

	ImageDownloadJob(QSet<QString> missingLayers, ImageTile incompleteTile)
		: WorkerJob(missingLayers, incompleteTile) { }

	ImageDownloadJob(WorkerJob from) : WorkerJob(from.missingLayers, from.incompleteTile) { }
};

struct ImageDownloadJobMetaData {
	ImageDownloadJob* job;
	QString layer;

	ImageDownloadJobMetaData() = default;
	ImageDownloadJobMetaData(ImageDownloadJob* job, QString layer)
		: job(job), layer(layer) { }
};

// make the ImageDownloadJob struct compatible with the QVariant type
Q_DECLARE_METATYPE(ImageDownloadJobMetaData);


/**
 * TileRequestWorker that downloads non-cached tiles as needed.
 */
class ClientTileRequestWorker : public TileRequestWorker {
	Q_OBJECT;

public:
	/**
	 * Creates a new ClientTileRequestWorker responsible for loading ImageTiles with the given
	 * layers from the cache if possible, or triggers downloads for them otherwise.
	 *
	 * @param layers            the ImageTile layers to fetch
	 * @param onTileFetched     callback that will be called when a tile was successfully loaded
	 * @param onTileFetchFailed callback that will be called on fetching failures
	 * @param configFile        path to the JSON file from which to read the layer config
	 */
	ClientTileRequestWorker(QSet<QString> layers, TileRequestWorker::OnTileFetched onTileFetched,
	                        TileRequestWorker::OnTileFetchFailed onTileFetchFailed,
	                        QString configFile = "./res/layers.json");

	virtual ~ClientTileRequestWorker();

	/**
	 * @param jobCount the maximum number of jobs to process simultaniously
	 */
	void setMaxJobCount(int jobCount);

	/**
	 * @returns the maximum number of jobs to process simultaniously
	 */
	int getMaxJobCount();

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
	/** Queue used to schedule jobs for the timer callback to process. */
	QQueue<WorkerJob> jobQueue;

	/** The set containing all pending download jobs. */
	QSet<ImageDownloadJob*> pendingDownloadJobs;

	/** Map used to associate a NetworkReply with ImageDownloadJobMetaData. */
	QMap<QNetworkReply*, ImageDownloadJobMetaData*> replyJobMetaMapping;

	/** The maximum number of jobs to process simultaniously. */
	int maxJobCount;

	/** Stores the ID of the shutdown handler, enabling us to unregister it later on. */
	int shutdownHandlerId;

	/**
	 * Removes the given job from the pendingDownloadsJobs list
	 *
	 * @param downloadJob the job to remove
	 */
	void removeDownloadJob(ImageDownloadJob* downloadJob);

	/** The network access manager used to make HTTP requests. */
	QNetworkAccessManager networkManager;

	/**
	 * Converts a raw bil16 encoded heightmap to a MetaImage.
	 *
	 * @param rawData the raw heightmap data
	 * @param width   the expected width of the image
	 * @param height  the expected height of the image
	 *
	 * @returns a greyscale heightmap with min and max height data attached
	 */
	static MetaImage decodeBil16(const QByteArray& rawData, int width, int height);

	/**
	 * Handles the given network reply.
	 *
	 * @param reply the reply received from the server
	 */
	void handleDownload(QNetworkReply* reply);

	/**
	 * Checks if the status code of the response is 200.
	 *
	 * @param reply the reply received from the server
	 */
	void checkStatusCode(QNetworkReply* reply);

	/**
	 * Handles the content of the given network reply.
	 *
	 * @param reply the reply received from the server
	 * @param meta  the metadata object associated with the reply
	 */
	void handleReplyContent(QNetworkReply* reply, ImageDownloadJobMetaData* meta);

private slots:
	/**
	 * Slot called whenever a download finished.
	 *
	 * @param reply the reply received from the server
	 */
	void downloadFinished(QNetworkReply* reply);

	/**
	 * Pops an item from the job queue and handles it.
	 */
	void processJobQueue();
};

#endif // KRONOS_UTILS_TILE_DOWNLOAD_CLIENT_TILE_REQUEST_WORKER_HPP
