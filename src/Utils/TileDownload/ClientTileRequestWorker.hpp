#ifndef KRONOS_UTILS_TILE_DOWNLOAD_CLIENT_TILE_REQUEST_WORKER_HPP
#define KRONOS_UTILS_TILE_DOWNLOAD_CLIENT_TILE_REQUEST_WORKER_HPP

#include <Utils/TileDownload/TileRequestWorker.hpp>

#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMetaType>


struct ImageDownloadJob : public WorkerJob {
    bool aborted = false;
    QSet<QNetworkReply*> pendingReplies;

    ImageDownloadJob() = default;

    ImageDownloadJob(QSet<QString> missingLayers, ImageTile incompleteTile)
            : WorkerJob(missingLayers, incompleteTile) { }

    ImageDownloadJob(WorkerJob from) : WorkerJob(from.missingLayers, from.incompleteTile) { }
};

struct ImageDownloadJobMetaData {
    std::shared_ptr<ImageDownloadJob> job;
    QString layer;

    ImageDownloadJobMetaData() = default;
    ImageDownloadJobMetaData(std::shared_ptr<ImageDownloadJob> job, QString layer)
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
     */
    ClientTileRequestWorker(QSet<QString> layers, TileRequestWorker::OnTileFetched onTileFetched,
                            TileRequestWorker::OnTileFetchFailed onTileFetchFailed);

    virtual ~ClientTileRequestWorker();

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

private:;
    /** The set containing all pending download jobs. */
    QList<std::shared_ptr<ImageDownloadJob>> pendingDownloadJobs;

    /**
     * Removes the given job from the pendingDownloadsJobs list
     *
     * @param downloadJob the job to remove
     */
    void removeDownloadJob(std::shared_ptr<ImageDownloadJob> downloadJob);

    /** The network access manager used to make HTTP requests. */
    QNetworkAccessManager networkManager;

    void handleDownload(QNetworkReply* reply);

private slots:
    void downloadFinished(QNetworkReply* reply);
};

#endif // KRONOS_UTILS_TILE_DOWNLOAD_CLIENT_TILE_REQUEST_WORKER_HPP
