#ifndef KRONOS_IMAGEDOWNLOADER_HPP
#define KRONOS_IMAGEDOWNLOADER_HPP

#include "ImageTile.hpp"
#include "ImageCache.hpp"
#include "ImageLayerDescription.hpp"

#include <functional>
#include <QImage>
#include <QFile>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QMap>

class ImageDownloader {
public:

	/**
	 * Type used for the tile fetched callback.
	 */
	typedef std::function<void(ImageTile& tile)> tileFetchedCbType;

	/**
	 * Create a new ImageDownloader using the standard configuration.
	 */
	ImageDownloader(tileFetchedCbType tileFetchedCb);

	/**
	 * Create a new ImageDownloader using the specified configuration.
	 * @param configuration The configuration of this downloader in JSON notation
	 */
	ImageDownloader(tileFetchedCbType tileFetchedCb, QMap<QString, ImageLayerDescription> imageLayers);
	~ImageDownloader();

	void getTile(int zoomLevel, int tileX, int tileY);
	void getTile(QString &layer, int zoomLevel, int tileX, int tileY);

	/**
	 * Get an image tile of a data layer.
	 * @param layer Unique identifier of an existing layer. Refer to the method
	 * getAvailableLayers() for a list of possible layers.
	 * @param zoomLevel Zoom level down the quad tree of images
	 * @param tileX Horizontal position of the requested tile
	 * @param tileY Vertical position of the requested tile
	 * @return The requested image tile as a QImage pointer
	 */
	void getTile(QList<QString> layers, int zoomLevel, int tileX, int tileY);

	/**
	 * Get a list of available image layers.
	 * @return A list of all available image layers as specified by the
	 * configuration file
	 */
	QList<QString> getAvailableLayers();

private:
	/**
	 * An object holding all configuration information.
	 */
	QMap<QString, ImageLayerDescription> imageLayers;

	/**
	 * Callback function to be called when a requested tile was downloaded or fetched from the file
	 * system.
	 */
	tileFetchedCbType tileFetchedCb;

	/**
	 * Network access manager used to make requests to the image API.
	 */
	static QNetworkAccessManager networkManager;

	/**
	 * Image cache used to save and retrieve downloaded tiles.
	 */
	static ImageCache imageCache;

	QString buildTileDownloadUrl(QString layer, int zoomLevel, int tileX, int tileY);

	/**
	 * Download an image from a specified URL.
	 * @param imageUrl URL of the image
	 * @return The image downloaded from the specified URL
	 */
	QImage downloadImageSync(QUrl& imageUrl);
};

#endif
