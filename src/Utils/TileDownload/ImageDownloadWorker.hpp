#ifndef KRONOS_IMAGEDOWNLOADWORKER_HPP
#define KRONOS_IMAGEDOWNLOADWORKER_HPP

#include <qbytearray.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qstring.h>
#include <qurl.h>
#include <qvariant.h>
#include <Utils/TileDownload/MetaImage.hpp>
#include <exception>
#include <future>
#include <string>
#include <thread>


/**
 * Base struct for all exceptions thrown whenever a download failed.
 */
struct DownloadFailedException : public std::exception {
	std::string reason;

	DownloadFailedException(QString reason) : reason(reason.toStdString()) { }

	const char *what() const noexcept override {
		return reason.c_str();
	}
};

/**
 * Exception thrown when there was a connection error while attempting a download.
 */
struct ConnectionFailedException : public DownloadFailedException {
	ConnectionFailedException(QUrl url, QNetworkReply::NetworkError error)
			: DownloadFailedException(
				QString("An error occurred during the network request. Url: '%1', Error code: %2. (See http://doc.qt.io/qt-4.8/qnetworkreply.html#NetworkError-enum)")
				.arg(url.toString(), error)
			) { }
};

/**
 * Exception thrown when a network request returned a status code other than 200 OK.
 */
struct BadStatusCodeException : public DownloadFailedException {
	BadStatusCodeException(QNetworkReply *reply)
			: DownloadFailedException(
				QString("The network request returned with an unexpected status code. Url: '%1', Status code: %2 %3.")
				.arg(
					reply->url().toString(),
					QString::number(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()),
					reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString()
				)
			) { }
};

/**
 * Exception thrown when a network request returned a content type that the ImageDownloadWorker
 * doesn't recognize.
 */
struct UnknownContentTypeException : public DownloadFailedException {
	UnknownContentTypeException(QString dataFormat, QUrl url)
			: DownloadFailedException(
				QString("The returned content type '%1' of '%2' cannot be processed.")
				.arg(dataFormat, url.toString())
			) { }
};

/**
 * Exception thrown when a request returned invalid image data.
 */
struct ImageDecodingFailedException : public DownloadFailedException {
	ImageDecodingFailedException(QUrl url)
			: DownloadFailedException(
				QString("Failed decoding the image returned from '%1'").arg(url.toString())
			) { }
};

/**
 * Exception thrown when a request returned malformed bil16 data.
 */
struct Bil16DecodingFailedException : public DownloadFailedException {
	Bil16DecodingFailedException(QString reason)
			: DownloadFailedException(
				QString("Error decoding bil16: %1").arg(reason)
			) { }
};


class ImageDownloadWorker {
public:
    /**
     * Creates a new ImageDownloadWorker responsible for downloading the given image, using the
     * provided QNetworkAccessManager.
     *
     * @param layerName      the name of the layer the downloaded image belongs to
     * @param url            the url of the image to download
     * @param networkManager the network access manager to use for the actual download
     */
    ImageDownloadWorker(QString layerName, QUrl url, int imageWidth, int imageHeight);

	ImageDownloadWorker(const ImageDownloadWorker&) = delete;

    /**
     * @returns the layer name provided in the constructor
     */
    QString getLayerName() const;

    /**
     * @returns the future that will eventually hold the downloaded MetaImage
     */
    std::future<MetaImage> getFuture();

private:
    QString layerName;
    QUrl url;
    std::promise<MetaImage> imageDownloadedPromise;
    QNetworkAccessManager networkManager;
    int imageWidth;
    int imageHeight;

	QNetworkReply *reply;
	std::thread downloadThread;

    /**
     * Kicks off the image's download.
     */
    void startDownload();

    /**
     * Decodes a raw data array holding bil16 data into a MetaImage.
     *
     * @param rawData the byte-array holding raw bil16 data
     *
     * @returns a MetaImage containing the normalized bil16 image, along with min and max heights
     */
    static MetaImage decodeBil16(const QByteArray& rawData, int width, int height);

    /**
     * Method that gets called when the download completed or an error occurred.
     */
    void downloadCompleted();
};

#endif
