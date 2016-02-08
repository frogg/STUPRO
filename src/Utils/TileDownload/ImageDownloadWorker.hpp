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
#include <Utils/TileDownload/DownloadExceptions.hpp>
#include <future>
#include <string>

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
	 * Aborts the currently running download.
	 */
	void abortDownload();

	/**
	 * Returns true if the network request is currently running.
	 */
	bool isRunning();

	/**
	 * Returns true if the network request has finished.
	 */
	bool isFinished();

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

	bool abortRequested;
	QNetworkReply* reply;

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
