#include <Utils/TileDownload/ClientTileRequestWorker.hpp>
#include <Utils/TileDownload/ImageCache.hpp>

#include <Utils/Misc/MakeUnique.hpp>
#include <Kronos.h>

#include <QNetworkRequest>
#include <QRegExp>
#include <QTimer>

ClientTileRequestWorker::ClientTileRequestWorker(QSet<QString> layers,
        TileRequestWorker::OnTileFetched onTileFetched,
        TileRequestWorker::OnTileFetchFailed onTileFetchFailed, QString configFile)
	: TileRequestWorker(layers, onTileFetched, onTileFetchFailed, configFile), maxJobCount(5) {

	// make sure all requests get cancelled when the plugin gets unloaded
	if (Kronos::getInstance()) {
		this->shutdownHandlerId = Kronos::getInstance()->registerShutdownHandler([this] {
			this->requestAbort();
		});
	}

	QObject::connect(
	    &this->networkManager, SIGNAL(finished(QNetworkReply*)),
	    this, SLOT(downloadFinished(QNetworkReply*))
	);

	QTimer* timer = new QTimer(this);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(processJobQueue()));
	timer->start(1);
}

ClientTileRequestWorker::~ClientTileRequestWorker() {
	if (Kronos::getInstance()) {
		Kronos::getInstance()->unregisterShutdownHandler(this->shutdownHandlerId);
	}
}

void ClientTileRequestWorker::scheduleJob(WorkerJob job) {
	this->jobQueue.append(job);
}

void ClientTileRequestWorker::processJobQueue() {
	if (this->jobQueue.isEmpty() || this->pendingDownloadJobs.size() >= this->getMaxJobCount()) {
		return;
	}

	WorkerJob job = this->jobQueue.dequeue();
	ImageDownloadJob* downloadJob = new ImageDownloadJob(job);

	int zoom = job.incompleteTile.getZoomLevel();
	int x = job.incompleteTile.getTileX();
	int y = job.incompleteTile.getTileY();

	// start downloads for every missing image
	for (auto layer : job.missingLayers) {
		QNetworkRequest req(this->layerConfig[layer].buildTileUrl(zoom, x, y));

		ImageDownloadJobMetaData* metaData = new ImageDownloadJobMetaData(downloadJob, layer);

		QNetworkReply* reply = this->networkManager.get(req);
		downloadJob->pendingReplies.insert(reply);

		this->replyJobMetaMapping[reply] = metaData;
	}

	this->pendingDownloadJobs.insert(downloadJob);
}

void ClientTileRequestWorker::handleAbortRequest() {
	// cancel jobs which aren't started yet
	this->jobQueue.clear();

	// abort all pending replies
	for (auto job : this->pendingDownloadJobs) {
		for (auto reply : job->pendingReplies) {
			reply->abort();
		}
	}
}

void ClientTileRequestWorker::removeDownloadJob(ImageDownloadJob* downloadJob) {
	QMutableSetIterator<ImageDownloadJob*> jobIterator(this->pendingDownloadJobs);
	if (jobIterator.findNext(downloadJob)) {
		jobIterator.remove();
	}
}

void ClientTileRequestWorker::downloadFinished(QNetworkReply* reply) {
	try {
		this->handleDownload(reply);
	} catch (std::exception const& e) {
		// we'll need to create a new exception to throw. If we just pass on the exception we just
		// catched, its copy constructor will be called, effectively slicing off the derived part
		// of the exception (which contains the actual error message)
		// doing so, we'll loose all type information on what exact error was thrown, but at least
		// the error message is correct
		this->onTileFetchFailed(KronosException(QString(e.what())));
	}

	// delete the reply as soon as control is returned to the event loop
	reply->deleteLater();
}

void ClientTileRequestWorker::handleDownload(QNetworkReply* reply) {
	// get and remove the metadata object from the reply -> replyJobMeta map. Putting it into a
	// unique_ptr will make sure that the object is deleted as soon as we leave the current scope.
	std::unique_ptr<ImageDownloadJobMetaData> meta(this->replyJobMetaMapping.take(reply));

	QString layer = meta->layer;
	ImageDownloadJob* job = meta->job;

	// mark the reply in the job as done
	job->pendingReplies.remove(reply);

	// remove the job from the job list if all pending replies returned
	if (job->pendingReplies.isEmpty()) {
		this->removeDownloadJob(job);
	}

	if (reply->error() == QNetworkReply::OperationCanceledError) {
		// make sure to only send one aborted exception
		if (!job->aborted) {
			job->aborted = true;
			throw DownloadAbortedException(reply->url());
		}
		return;
	}

	// check status code
	this->checkStatusCode(reply);

	// handle content
	this->handleReplyContent(reply, meta.get());

	if (job->pendingReplies.isEmpty()) {
		ImageTile tile = job->incompleteTile;

		// the job is finished, so delete it
		delete job;

		// ensure all layers are present
		bool allLayersPresent = true;
		for (QString layer : this->layers) {
			if (!tile.getLayers().contains(layer)) {
				allLayersPresent = false;
				break;
			}
		}

		if (allLayersPresent) {
			this->onTileFetched(tile);
		} else {
			throw TileIncompleteException(tile.getLayers().keys(), this->layers);
		}
	}
}

void ClientTileRequestWorker::checkStatusCode(QNetworkReply* reply) {
	QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	if (!statusCode.isValid()) {
		throw InvalidReplyException();
	}
	if (statusCode.toInt() != 200) {
		throw BadStatusCodeException(reply);
	}
}

void ClientTileRequestWorker::handleReplyContent(QNetworkReply* reply,
        ImageDownloadJobMetaData* meta) {
	std::unique_ptr<MetaImage> metaImage = nullptr;

	meta->job->missingLayers.remove(meta->layer);

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

		int imageSize = this->layerConfig[meta->layer].getTileSize();
		MetaImage image = ClientTileRequestWorker::decodeBil16(rawData, imageSize, imageSize);

		// store the image
		metaImage = makeUnique<MetaImage>(image);
	} else {
		throw UnknownContentTypeException(contentType, reply->url());
	}

	if (metaImage != nullptr) {
		// add the image to the incomplete tile
		meta->job->incompleteTile.getLayers()[meta->layer] = *metaImage;

		// cache the image
		ImageTile tile = meta->job->incompleteTile;
		ImageCache::getInstance().cacheImage(*metaImage, meta->layer, tile.getZoomLevel(), tile.getTileX(),
		                                     tile.getTileY());
	}
}

void ClientTileRequestWorker::setMaxJobCount(int jobCount) {
	if (jobCount >= 1) {
		this->maxJobCount = jobCount;
	}
}

int ClientTileRequestWorker::getMaxJobCount() {
	return this->maxJobCount;
}

MetaImage ClientTileRequestWorker::decodeBil16(const QByteArray& rawData, int width, int height) {
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
