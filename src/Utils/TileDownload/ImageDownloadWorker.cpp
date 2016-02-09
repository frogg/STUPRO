#include <Utils/TileDownload/ImageDownloadWorker.hpp>

#include <climits>
#include <thread>
#include <vector>
#include <QEventLoop>
#include <QRegExp>

ImageDownloadWorker::ImageDownloadWorker(QString layerName, QUrl url, int imageWidth,
        int imageHeight)
	: layerName(layerName), url(url), imageWidth(imageWidth), imageHeight(imageHeight),
	  abortRequested(false), reply(0) {
	this->startDownload();
}

void ImageDownloadWorker::abortDownload() {
	this->abortRequested = true;
	if (this->reply && this->reply->isRunning()) {
		this->reply->close();
	}
}

bool ImageDownloadWorker::isRunning() {
	return this->reply && this->reply->isRunning();
}

bool ImageDownloadWorker::isFinished() {
	return this->reply && this->reply->isFinished();
}

QString ImageDownloadWorker::getLayerName() const {
	return this->layerName;
}

std::future<MetaImage> ImageDownloadWorker::getFuture() {
	return this->imageDownloadedPromise.get_future();
}

void ImageDownloadWorker::startDownload() {
	// create and detach a new thread that triggers the download of the image and waits until the
	// download completed
	std::thread([this]() {
		// the reason why the downloadCompleted() call is present on both sides of the if-statement
		// (versus a single call after the if-else) is that for some reason, the call to
		// reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) in the downloadCompleted()
		// method will throw a SIGSEGV otherwise.

		if (this->abortRequested) {
			this->downloadCompleted();
		} else {
			// event loop needed for the network access manager and the synchronous request
			QEventLoop replyLoop;

			// trigger the request
			QNetworkAccessManager networkManager;
			QNetworkRequest request(this->url);
			this->reply = networkManager.get(request);

			// wait for the download to complete
			QObject::connect(this->reply, SIGNAL(finished()), &replyLoop, SLOT(quit()));
			replyLoop.exec();

			// notify the ImageDownloadWorker of the completed download
			this->downloadCompleted();
		}
	}).detach();
}

MetaImage ImageDownloadWorker::decodeBil16(const QByteArray& rawData, int width, int height) {
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

void ImageDownloadWorker::downloadCompleted() {
	try {
		if (reply) {
			if (reply->error()) {
				if (reply->error() == QNetworkReply::OperationCanceledError) {
					throw DownloadAbortedException(this->url);
				}

				throw ConnectionFailedException(this->url, this->reply->error());
			}
			QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
			if (!statusCode.isValid()) {
				throw InvalidReplyException();
			}
			if (statusCode.toInt() != 200) {
				throw BadStatusCodeException(this->reply);
			}
		}
		if (this->abortRequested) {
			throw DownloadAbortedException(this->url);
		}

		QString contentType = this->reply->header(QNetworkRequest::ContentTypeHeader).toString();
		if (QRegExp("^image\\/(png|tiff|jpeg|gif)$").exactMatch(contentType)) {
			QByteArray rawData = this->reply->readAll();

			QImage image = QImage::fromData(rawData);

			// ensure that the image was read
			if (image.isNull()) {
				throw ImageDecodingFailedException(this->url);
			}

			MetaImage metaImage(image);
			this->imageDownloadedPromise.set_value(metaImage);
		} else if (QRegExp("^application\\/bil16$").exactMatch(contentType)) {
			QByteArray rawData = this->reply->readAll();

			MetaImage metaImage = ImageDownloadWorker::decodeBil16(rawData, this->imageWidth,
			                      this->imageHeight);
			this->imageDownloadedPromise.set_value(metaImage);
		} else {
			throw UnknownContentTypeException(contentType, this->url);
		}
	} catch (...) {
		this->imageDownloadedPromise.set_exception(std::current_exception());
	}
}
