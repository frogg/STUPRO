#ifndef KRONOS_UTILS_TILE_DOWNLOAD_DOWNLOAD_EXCEPTIONS_HPP
#define KRONOS_UTILS_TILE_DOWNLOAD_DOWNLOAD_EXCEPTIONS_HPP

#include <Utils/Misc/Exceptions.hpp>

#include <QString>
#include <QUrl>
#include <QNetworkReply>
#include <exception>

/**
 * Base struct for all exceptions thrown whenever a download failed.
 */
struct DownloadFailedException : public KronosException {
	DownloadFailedException(QString reason) : KronosException(reason) { }
};

/**
 * Exception thrown when the download was cancelled before it finished.
 */
struct DownloadAbortedException : public DownloadFailedException {
	DownloadAbortedException() : DownloadFailedException("The download was aborted.") { }

	DownloadAbortedException(QUrl url)
		: DownloadFailedException(
		      QString("The download was aborted. Url: '%1'").arg(url.toString())
		  ) { }
};

/**
 * Exception thrown when there was a connection error while attempting a download.
 */
struct ConnectionFailedException : public DownloadFailedException {
	ConnectionFailedException(QUrl url, QNetworkReply::NetworkError error)
		: DownloadFailedException(
		      QString("An error occurred during the network request. Url: '%1', Error code: %2. (See http://doc.qt.io/qt-4.8/qnetworkreply.html#NetworkError-enum)")
		      .arg(url.toString(), QString::number(error))
		  ) { }
};

/**
 * Exception thrown when a network request returned a status code other than 200 OK.
 */
struct BadStatusCodeException : public DownloadFailedException {
	BadStatusCodeException(QNetworkReply* reply)
		: DownloadFailedException(
		      QString("The network request returned with an unexpected status code. Url: '%1', Status code: %2 %3.")
		      .arg(
		          reply->url().toString(),
		          QString::number(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()),
		          reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString()
		      )
		  ) { }
};

struct InvalidReplyException : public DownloadFailedException {
	InvalidReplyException()
		: DownloadFailedException(
		      QString("An invalid reply was received")
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

/**
 * Exception thrown when a non-existing layer is requested.
 */
struct InvalidLayerException : public KronosException {
	InvalidLayerException(QString givenLayer, QList<QString> availableLayers) {
		QString message("The given layer wasn't recognized. Expected one of { ");
		for (int i = 0; i < availableLayers.size(); i++) {
			message += "'" + availableLayers[i] + "'";
			if (i < availableLayers.size() - 1) {
				message += ", ";
			}
		}
		message += " }. Was given '" + givenLayer + "'.";
		this->reason = message.toStdString();
	}
};

/**
 * Exception thrown when a tile location parameter is out of range.
 */
struct InvalidTileLocationException : public KronosException {
	InvalidTileLocationException(QString message) : KronosException(message) { }
};

/**
 * Exception thrown when a tile's zoom level is out of range.
 */
struct InvalidTileZoomException : public InvalidTileLocationException {
	InvalidTileZoomException(int given, int min, int max)
		: InvalidTileLocationException(
		      QString("The given zoomLevel is invalid. Expected value between (including) %2 and %3. Was given %1.")
		      .arg(given, min, max)
		  ) { }
};

/**
 * Exception thrown when a tile's x or y position is out of range.
 */
struct InvalidTilePositionException : public InvalidTileLocationException {
	InvalidTilePositionException(int zoomLevel, int xMin, int xMax, int xActual, int yMin, int yMax,
	                             int yActual)
		: InvalidTileLocationException(
		      QString("The given tile position is invalid. Valid tile positions for zoom level %1 must be within x%2 to x%3 and y%5 to y%6. Was given x%4 y%7.")
		      .arg(
		          QString::number(zoomLevel),
		          QString::number(xMin), QString::number(xMax), QString::number(xActual),
		          QString::number(yMin), QString::number(yMax), QString::number(yActual)
		      )
		  ) { }
};

#endif
