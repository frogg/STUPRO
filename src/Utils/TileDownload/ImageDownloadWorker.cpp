#include <Utils/TileDownload/ImageDownloadWorker.hpp>

#include <climits>
#include <vector>
#include <QEventLoop>
#include <QRegExp>

ImageDownloadWorker::ImageDownloadWorker(QString layerName, QUrl url, int imageWidth,
		int imageHeight)
		: layerName(layerName), url(url), imageWidth(imageWidth), imageHeight(imageHeight),
		reply(NULL) {
	this->startDownload();
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
	}).detach();
}

MetaImage ImageDownloadWorker::decodeBil16(const QByteArray &rawData, int width, int height) {
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
		if (reply->error()) {
			throw ConnectionFailedException(this->url, this->reply->error());
		}
		if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
			throw BadStatusCodeException(this->reply);
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
