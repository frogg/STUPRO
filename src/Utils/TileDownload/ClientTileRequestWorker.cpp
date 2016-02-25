#include <Utils/TileDownload/ClientTileRequestWorker.hpp>

#include <QNetworkRequest>

ClientTileRequestWorker::ClientTileRequestWorker(QSet<QString> layers,
                                                 TileRequestWorker::OnTileFetched onTileFetched,
                                                 TileRequestWorker::OnTileFetchFailed onTileFetchFailed)
        : TileRequestWorker(layers, onTileFetched, onTileFetchFailed) {
    this->connect(&this->networkManager, SIGNAL(finished()), SLOT(downloadFinished()));
}

ClientTileRequestWorker::~ClientTileRequestWorker() {

}

void ClientTileRequestWorker::scheduleJob(WorkerJob job) {
    std::shared_ptr<ImageDownloadJob> downloadJob = std::make_shared<ImageDownloadJob>(job);

    int zoom = job.incompleteTile.getZoomLevel();
    int x = job.incompleteTile.getTileX();
    int y = job.incompleteTile.getTileY();

    // start downloads for every missing image
    for (auto layer : job.missingLayers) {
        QNetworkRequest req(this->layerConfig[layer].buildTileUrl(zoom, x, y));

        QVariant meta;
        meta.setValue(ImageDownloadJobMetaData(downloadJob, layer));
        req.setAttribute(QNetworkRequest::User, meta);

        downloadJob->pendingReplies.insert(this->networkManager.get(req));
    }

    this->pendingDownloadJobs.append(downloadJob);
}

void ClientTileRequestWorker::handleAbortRequest() {
    // abort all pending replies
    for (auto job : this->pendingDownloadJobs) {
        for (auto reply : job->pendingReplies) {
            reply->abort();
        }
    }
}

void ClientTileRequestWorker::removeDownloadJob(std::shared_ptr<ImageDownloadJob> downloadJob) {
    QMutableListIterator<std::shared_ptr<ImageDownloadJob>> jobIterator(this->pendingDownloadJobs);
    if (jobIterator.findNext(downloadJob)) {
        jobIterator.remove();
    }
}

void ClientTileRequestWorker::handleDownload(QNetworkReply* reply) {
    QVariant metadata = reply->attribute(QNetworkRequest::User);
    ImageDownloadJobMetaData meta = metadata.value<ImageDownloadJobMetaData>();

    // remove the reply from the pending reply set
    meta.job->pendingReplies.remove(reply);

    // remove the job from the job list if all pending replies returned
    if (meta.job->pendingReplies.isEmpty()) {
        this->removeDownloadJob(meta.job);
    }

    if (reply->error() == QNetworkReply::OperationCanceledError) {
        // make sure to only send one aborted exception
        if (!meta.job->aborted) {
            meta.job->aborted = true;
            throw DownloadAbortedException(reply->url());
        }
        return;
    }

    // check status code

    // check content type

}

void ClientTileRequestWorker::downloadFinished(QNetworkReply* reply) {
    try {
        this->handleDownload(reply);
    } catch (std::exception const& e) {
        this->onTileFetchFailed(e);
    }
}
