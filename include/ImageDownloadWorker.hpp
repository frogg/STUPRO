#ifndef KRONOS_IMAGEDOWNLOADWORKER_HPP
#define KRONOS_IMAGEDOWNLOADWORKER_HPP

#include "MetaImage.hpp"

#include <exception>
#include <future>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QUrl>


struct DownloadFailedException : public std::exception { };

struct ConnectionFailedException : public DownloadFailedException {
	QUrl url;
	QNetworkReply::NetworkError error;

	ConnectionFailedException(QUrl url, QNetworkReply::NetworkError error) : url(url), error(error) { }

	const char * what() const throw() {
		QString message("An error occurred during the network request. Url: '%1', Error code: %2.");
		message += " (See http://doc.qt.io/qt-4.8/qnetworkreply.html#NetworkError-enum)";
		return message.arg(url.toString(), error).toStdString().c_str();
	}
};

struct BadStatusCodeException : public DownloadFailedException {
	QNetworkReply *reply;

	BadStatusCodeException(QNetworkReply *reply) : reply(reply) { }

	const char * what() const throw() {
		QString message("The network request returned with an unexpected status code. Url: '%1'");
		message += ", Status code: %2 %3.";
		int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		QString statusMessage = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
		message = message.arg(reply->url().toString(), QString::number(statusCode), statusMessage);
		return message.toStdString().c_str();
	}
};

struct UnknownContentTypeException : public DownloadFailedException {
	QString dataFormat;
    QUrl url;

	UnknownContentTypeException(QString dataFormat, QUrl url) : dataFormat(dataFormat), url(url) { }

	const char * what() const throw() {
		QString message("The returned content type '%1' of '%2' cannot be processed.");
		return message.arg(dataFormat, url.toString()).toStdString().c_str();
	}
};

struct ImageDecodingFailedException : public DownloadFailedException {
	QUrl url;

	ImageDecodingFailedException(QUrl url) : url(url) { }

	const char * what() const throw() {
		QString message("Failed decoding the image returned from '%1'");
		return message.arg(url.toString()).toStdString().c_str();
	}
};

struct Bil16DecodingFailedException : public DownloadFailedException {
	QString reason;

	Bil16DecodingFailedException(QString reason) : reason(reason) { }

	const char * what() const throw() {
		return ("Error decoding bil16: " + reason).toStdString().c_str();
	}
};


class ImageDownloadWorker : public QObject {
    Q_OBJECT

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
    ~ImageDownloadWorker();

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

	QNetworkRequest request;
	QNetworkReply *reply;

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

private slots:
    /**
     * Method that gets called when the download completed or an error occurred.
     */
    void downloadCompleted();
};

#endif
