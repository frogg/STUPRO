#ifndef KRONOS_IMAGEDOWNLOADER_HPP
#define KRONOS_IMAGEDOWNLOADER_HPP

#include "ImageTile.hpp"

#include <functional>
#include <QImage>
#include <QFile>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QList>
#include <QJsonObject>

class ImageDownloader {
public:

	/**
	 * Type used for the image fetched callback.
	 */
	typedef std::function<void(ImageTile& tile)> tileFetchedCbType;

	/**
	 * Create a new ImageDownloader using the standard configuration.
	 */
	ImageDownloader(tileFetchedCbType imageFetchedCb);

	/**
	 * Create a new ImageDownloader using the specified configuration.
	 * @param configuration The configuration of this downloader in JSON notation
	 */
	ImageDownloader(tileFetchedCbType imageFetchedCb, QJsonObject* configuration);
	~ImageDownloader();

	/**
	 * Get an image tile of a data layer.
	 * @param layer Unique identifier of an existing layer. Refer to the method
	 * getAvailableLayers() for a list of possible layers.
	 * @param zoomLevel Zoom level down the quad tree of images
	 * @param tileX Horizontal position of the requested tile
	 * @param tileY Vertical position of the requested tile
	 * @return The requested image tile as a QImage pointer
	 */
	QImage* getImage(QString* layer, int zoomLevel, int tileX, int tileY);

	/**
	 * Get a list of available image layers.
	 * @return A list of all available image layers as specified by the
	 * configuration file
	 */
	QList<QString> getAvailableLayers();

private:
	/**
	 * A JSON object holding all configuration information.
	 */
	QJsonObject *config;

	/**
	 * Callback function to be called when a requested tile was downloaded or fetched from the file
	 * system.
	 */
	tileFetchedCbType imageFetchedCb;

	/**
	 * Network access manager used to make requests to the image API.
	 */
	static QNetworkAccessManager networkManager;

	/**
	 * Download an image from a specified URL.
	 * @param imageUrl URL of the image
	 * @return The image downloaded from the specified URL
	 */
	QImage* downloadImage(QUrl* imageUrl);

};

#endif
