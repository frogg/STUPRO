#include <Utils/TileDownload/ClientTileRequestWorker.hpp>

#include <Utils/Misc/MakeUnique.hpp>
#include <Utils/Misc/KronosLogger.hpp>

#include <QNetworkRequest>
#include <QRegExp>
#include <QTimer>

ClientTileRequestWorker::ClientTileRequestWorker(QSet<QString> layers,
                                                 TileRequestWorker::OnTileFetched onTileFetched,
                                                 TileRequestWorker::OnTileFetchFailed onTileFetchFailed)
        : TileRequestWorker(layers, onTileFetched, onTileFetchFailed) {
    QObject::connect(
        &this->networkManager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(downloadFinished(QNetworkReply*))
    );

    QTimer* timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(processJobQueue()));
    timer->start();
}

ClientTileRequestWorker::~ClientTileRequestWorker() { }

void ClientTileRequestWorker::scheduleJob(WorkerJob job) {
    KRONOS_LOG_DEBUG("scheduling download job for %d, (%d,%d)", job.incompleteTile.getZoomLevel(), job.incompleteTile.getTileX(), job.incompleteTile.getTileY());

    this->jobQueue.append(job);
}

void ClientTileRequestWorker::processJobQueue() {
    if (this->jobQueue.isEmpty()) {
        return;
    }

    WorkerJob job = this->jobQueue.dequeue();
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
    KRONOS_LOG_DEBUG("downloader received abort");
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

void ClientTileRequestWorker::downloadFinished(QNetworkReply* reply) {
    KRONOS_LOG_DEBUG("download finished");
    try {
        this->handleDownload(reply);
    } catch (std::exception const& e) {
        KRONOS_LOG_DEBUG("download finished with exception: %s", e.what());
        // only send one exception per tile
        std::shared_ptr<ImageDownloadJob> job = ClientTileRequestWorker::getMetaData(reply).job;
        if (!job->failed) {
            job->failed = true;
            this->onTileFetchFailed(e);
        }
    }
}

void ClientTileRequestWorker::handleDownload(QNetworkReply* reply) {
    KRONOS_LOG_DEBUG("handling download");
    ImageDownloadJobMetaData meta = ClientTileRequestWorker::getMetaData(reply);

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
    this->checkStatusCode(reply);

    // handle content
    this->handleReplyContent(reply);

    if (!meta.job->failed && meta.job->pendingReplies.isEmpty()) {
        // ensure all layers are present
        bool allLayersPresent = true;
        for (QString layer : this->layers) {
            if (!meta.job->incompleteTile.getLayers().contains(layer)) {
                allLayersPresent = false;
                break;
            }
        }
        if (allLayersPresent) {
            this->onTileFetched(meta.job->incompleteTile);
        } else {
            throw TileIncompleteException(meta.job->incompleteTile.getLayers().keys(), this->layers);
        }
    }
}

void ClientTileRequestWorker::checkStatusCode(QNetworkReply *reply) {
    KRONOS_LOG_DEBUG("checking download status code");
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (!statusCode.isValid()) {
        throw InvalidReplyException();
    }
    if (statusCode.toInt() != 200) {
        throw BadStatusCodeException(reply);
    }
}

void ClientTileRequestWorker::handleReplyContent(QNetworkReply *reply) {
    KRONOS_LOG_DEBUG("handling reply content");
    ImageDownloadJobMetaData meta = ClientTileRequestWorker::getMetaData(reply);
    std::unique_ptr<MetaImage> metaImage = nullptr;

    meta.job->missingLayers.remove(meta.layer);

    QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
    if (QRegExp("^image\\/(png|tiff|jpeg|gif)$").exactMatch(contentType)) {
        QByteArray rawData = reply->readAll();

        QImage image = QImage::fromData(rawData);

        // ensure that the image was read
        if (image.isNull()) {
            throw ImageDecodingFailedException(reply->url());
        }

        // store the image
        metaImage = makeUnique<MetaImage>(image);
    } else if (QRegExp("^application\\/bil16$").exactMatch(contentType)) {
        QByteArray rawData = reply->readAll();

        int imageSize = this->layerConfig[meta.layer].getTileSize();
        MetaImage image = ClientTileRequestWorker::decodeBil16(rawData, imageSize, imageSize);

        // store the image
        metaImage = makeUnique<MetaImage>(image);
    } else {
        throw UnknownContentTypeException(contentType, reply->url());
    }

    if (metaImage != nullptr) {
        meta.job->incompleteTile.getLayers()[meta.layer] = *metaImage;
    }
}

ImageDownloadJobMetaData ClientTileRequestWorker::getMetaData(QNetworkReply* reply) {
    KRONOS_LOG_DEBUG("getting download metadata");
    QVariant metadata = reply->attribute(QNetworkRequest::User);
    ImageDownloadJobMetaData meta = metadata.value<ImageDownloadJobMetaData>();
    return meta;
}

MetaImage ClientTileRequestWorker::decodeBil16(const QByteArray &rawData, int width, int height) {
    KRONOS_LOG_DEBUG("decoding bil16");
    // ensure the raw data contains two bytes of data for every pixel of the image
	if (rawData.size() != width * height * 2) {
		QString errorMessage("Expected raw data length of %1b, but %2b were given");
		throw Bil16DecodingFailedException(errorMessage.arg(width * height * 2, rawData.size()));
	}
	short minHeight = SHRT_MAX;
	short maxHeight = SHRT_MIN;

	std::vector<short> heightValues(width * height);

	// find min and max values used to normalize the image in the second pass
	int rawIndex = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			short heightValue = (rawData[rawIndex + 1] << 8) | (unsigned char)rawData[rawIndex];
			if (heightValue < minHeight) {
				minHeight = heightValue;
			}
			if (heightValue > maxHeight) {
				maxHeight = heightValue;
			}

			heightValues[x + y * width] = heightValue;
			rawIndex += 2;
		}
	}

	// use min and max heights to normalize the data and save it in a QImage
	// color values of 0xff correspond to maxHeight, 0x00 means minHeight
	float normalizationFactor = 255.0 / (maxHeight - minHeight);
	QImage image(width, height, QImage::Format_RGB32);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			short heightValue = heightValues[x + y * width];
			short normalizedHeight = (heightValue - minHeight) * normalizationFactor;
			QRgb pixelValue = qRgb(normalizedHeight, normalizedHeight, normalizedHeight);
			image.setPixel(x, y, pixelValue);
		}
	}

	return MetaImage(image, minHeight, maxHeight);
}
