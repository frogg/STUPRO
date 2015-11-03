#include "../include/ImageDownloadWorker.hpp"

#include <climits>
#include <QRegExp>

#include "../include/DebugLogger.hpp"

ImageDownloadWorker::ImageDownloadWorker(QString layerName, QUrl url, int imageWidth,
		int imageHeight) : layerName(layerName), url(url), imageWidth(imageWidth),
	imageHeight(imageHeight), request(QNetworkRequest(url)) {
	this->startDownload();
}

ImageDownloadWorker::~ImageDownloadWorker() { }

QString ImageDownloadWorker::getLayerName() const {
	return this->layerName;
}

std::future<MetaImage> ImageDownloadWorker::getFuture() {
	return this->imageDownloadedPromise.get_future();
}

void ImageDownloadWorker::startDownload() {
	DebugLogger::debug(QString("creating request for ") + this->url.toString());
	this->reply = this->networkManager.get(request);
	QObject::connect(this->reply, SIGNAL(readyRead()), this, SLOT(downloadCompleted()));
	DebugLogger::debug("download slot connected");
	while (!this->reply->isFinished()) { }
	DebugLogger::debug("reply->isFinished() == true");
}

MetaImage ImageDownloadWorker::decodeBil16(const QByteArray &rawData, int width, int height) {
	if (!rawData.size() == width * height * 2) {
		QString errorMessage("Expected raw data length of %1b, but %2b were given");
		throw Bil16DecodingFailedException(errorMessage.arg(width * height * 2, rawData.size()));
	}
	short minHeight = SHRT_MAX;
	short maxHeight = SHRT_MIN;

	// find min and max values used to normalize the image in the second pass
	int rawIndex = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			short heightValue = (rawData[rawIndex] << 8) | (unsigned char)rawData[rawIndex + 1];
			if (heightValue < minHeight) {
				minHeight = heightValue;
			}
			if (heightValue > maxHeight) {
				maxHeight = heightValue;
			}
			rawIndex += 2;
		}
	}

	// use min and max heights to normalize the data and save it in a QImage
	// color values of 0xff correspond to maxHeight, 0x00 means minHeight
	QImage image(width, height, QImage::Format_ARGB32);
	rawIndex = 0;
	float normalizationFactor = (maxHeight - minHeight) / 255.0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			short heightValue = (rawData[rawIndex] << 8) | (unsigned char)rawData[rawIndex + 1];
			short normalizedHeight = (heightValue - minHeight) * normalizationFactor;

			QRgb color = qRgba(normalizedHeight, normalizedHeight, normalizedHeight, normalizedHeight);
			image.setPixel(x, y, color);

			rawIndex += 2;
		}
	}

	return MetaImage(image, minHeight, maxHeight);
}

void ImageDownloadWorker::downloadCompleted() {
	DebugLogger::debug("download completed");
	try {
		if (reply->error()) {
			throw ConnectionFailedException(this->url, this->reply->error());
		}
		if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
			throw BadStatusCodeException(this->reply);
		}

		QString contentType = this->reply->header(QNetworkRequest::ContentTypeHeader).toString();
		MetaImage metaImage;
		if (QRegExp("^image\\/(png|tiff|jpeg|gif)$").exactMatch(contentType)) {
			QByteArray rawData = this->reply->readAll();

			QImage image = QImage::fromData(rawData);

			// ensure that the image was read
			if (image.isNull()) {
				throw ImageDecodingFailedException(this->url);
			}

			metaImage = MetaImage(image);
		} else if (QRegExp("^application\\/bil16$").exactMatch(contentType)) {
			QByteArray rawData = this->reply->readAll();

			metaImage = ImageDownloadWorker::decodeBil16(rawData, this->imageWidth, this->imageHeight);
		} else {
			throw UnknownContentTypeException(contentType, this->url);
		}
		this->imageDownloadedPromise.set_value_at_thread_exit(metaImage);
	} catch (...) {
		this->imageDownloadedPromise.set_exception_at_thread_exit(std::current_exception());
	}
}
