#ifndef KRONOS_IMAGEDOWNLOADER_HPP
#define KRONOS_IMAGEDOWNLOADER_HPP

#include "ImageTile.hpp"
#include "ImageCache.hpp"
#include "ImageLayerDescription.hpp"
#include "MetaImage.hpp"

#include <exception>
#include <functional>
#include <QFile>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QMap>

struct InvalidTileLocationException : public std::exception { };

struct InvalidTileZoomException : public InvalidTileLocationException {
	int givenZoomLevel;
	int minZoomLevel;
	int maxZoomLevel;

	InvalidTileZoomException(int given, int min, int max)
		: givenZoomLevel(given), minZoomLevel(min), maxZoomLevel(max) { }

	const char * what() const throw() {
		QString message("The given zoomLevel is invalid. Expected value between (including) %1 and %2. Was given %3.");
		message = message.arg(minZoomLevel, maxZoomLevel, givenZoomLevel);

		return message.toStdString().c_str();
	}
};

struct InvalidTilePositionException : public InvalidTileLocationException {
	int zoomLevel;
	int xMin, xMax, xActual;
	int yMin, yMax, yActual;

	InvalidTilePositionException(int zoomLevel, int xMin, int xMax, int xActual, int yMin, int yMax, int yActual)
		: zoomLevel(zoomLevel), xMin(xMin), xMax(xMax), xActual(xActual),
		  yMin(yMin), yMax(yMax), yActual(yActual) { }

	const char * what() const throw() {
		QString message("The given tile position is invalid. Valid tile positions for zoom level %1 must be within x%2 to x%3 and y%5 to y%6. Was given x%4 y%7.");
		// message = message.arg(QString::number(zoomLevel), xMin, xMax, xActual, yMin, yMax, yActual);
		message = message.arg(zoomLevel);
		message = message.arg(xMin, xMax, xActual);
		message = message.arg(yMin, yMax, yActual);

		return message.toStdString().c_str();
	}
};

class ImageDownloader {
public:
	/**
	 * Type used for the tile fetched callback.
	 */
	typedef std::function<void(ImageTile& tile)> TileFetchedCb;

	static const int MIN_ZOOM_LEVEL = 0;
	static const int MAX_ZOOM_LEVEL = 15;
	static const int WIDTH_AT_MIN_ZOOM = 8;
	static const int HEIGHT_AT_MIN_ZOOM = 4;

	/**
	 * Creates a new ImageDownloader using the default configuration.
	 *
	 * @param tileFetchedCb the callback to call whenever a tile has finished loading.
	 */
	ImageDownloader(TileFetchedCb tileFetchedCb);

	/**
	 * Creates a new ImageDownloader using the given configuration.
	 *
	 * @param tileFetchedCb the callback to call whenever a tile has finished loading.
	 * @param imageLayers   a map containing layername - layerdescription objects to be used for
	 *                      loading the tiles.
	 */
	ImageDownloader(TileFetchedCb tileFetchedCb, QMap<QString, ImageLayerDescription> imageLayers);
	~ImageDownloader();

	/**
	 * Fetches images of all layers at the given location.
	 * The tile fetched callback given to the constructor will be called as soon as all images are
	 * loaded.
	 *
	 * @param zoomLevel how deep to dive into the quad-tree
	 * @param tileX     horizontal position of the requested tile (westernmost tile = 0)
	 * @param tileY     vertical position of the requested tile (northernmost tile = 0)
	 */
	void getTile(int zoomLevel, int tileX, int tileY);

	/**
	 * Fetches the image of the given layer at the given location.
	 * @see {#getAvailableLayers} for a list of all available layers. The tile fetched callback
	 * given to the constructor will be called as soon as all images are loaded.
	 *
	 * @param layer     the layer to get the image from
	 * @param zoomLevel how deep to dive into the quad-tree
	 * @param tileX     horizontal position of the requested tile (westernmost tile = 0)
	 * @param tileY     vertical position of the requested tile (northernmost tile = 0)
	 */
	void getTile(QString &layer, int zoomLevel, int tileX, int tileY);

	/**
	 * Fetches the images of the given layers at the given location.
	 * @see {#getAvailableLayers} for a list of all available layers. The tile fetched callback
	 * given to the constructor will be called as soon as all images are loaded.
	 *
	 * @param layers    the layers to get the images from
	 * @param zoomLevel how deep to dive into the quad-tree
	 * @param tileX     horizontal position of the requested tile (westernmost tile = 0)
	 * @param tileY     vertical position of the requested tile (northernmost tile = 0)
	 */
	void getTile(QList<QString> layers, int zoomLevel, int tileX, int tileY);

	/**
	 * Returns a list of available image layers.
	 *
	 * @returns a list containing the names of all available image layers as read from the config
	 *          file
	 */
	QList<QString> getAvailableLayers();

private:
	/**
	 * Type used for the image downloaded callback.
	 */
	typedef std::function<void(MetaImage)> ImageDownloadedCb;

	/**
	 * An object holding all configuration information.
	 */
	QMap<QString, ImageLayerDescription> imageLayers;

	/**
	 * Callback function to be called when a requested tile was downloaded or fetched from the file
	 * system.
	 */
	TileFetchedCb tileFetchedCb;

	/**
	 * Network access manager used to make requests to the image API.
	 */
	static QNetworkAccessManager networkManager;

	/**
	 * Chechs if the given tile location is valid for the underlying quad-tree.
	 *
	 * @param zoomLevel how deep to dive into the quad-tree
	 * @param tileX     horizontal position of the requested tile (westernmost tile = 0)
	 * @param tileY     vertical position of the requested tile (northernmost tile = 0)
	 *
	 * @throws InvalidTileZoomException if the zoom level is out of bounds
	 * @throws InvalidTilePosition      if the x or y position of the tile is out of bounds for the
	 *                                  given zoom level
	 */
	static void validateTileLocation(int zoomLevel, int tileX, int tileY);

	/**
	 * Calculates the bounding box in lat-long from the given tile location.
	 * Assumes that the given tile location is valid.
	 *
	 * @param zoomLevel how deep to dive into the quad-tree
	 * @param tileX     horizontal position of the requested tile (westernmost tile = 0)
	 * @param tileY     vertical position of the requested tile (northernmost tile = 0)
	 *
	 * @returns a string representation of the lat-long bounding box
	 */
	static QString calculateBoundingBox(int zoomLevel, int tileX, int tileY);

	/**
	 * Returns the URL at which the image with the given properties can be found.
	 *
	 * @param layer     the layer from which to get the image
	 * @param zoomLevel how deep to dive into the quad-tree
	 * @param tileX     horizontal position of the requested tile (westernmost tile = 0)
	 * @param tileY     vertical position of the requested tile (northernmost tile = 0)
	 *
	 * @returns a URL pointing to the image with the given properties
	 */
	QUrl buildTileDownloadUrl(QString layer, int zoomLevel, int tileX, int tileY);

	/**
	 * Download an image from a specified URL.
	 *
	 * @param imageUrl URL of the image
	 * @return The image downloaded from the specified URL
	 */
	void downloadImage(QUrl &imageUrl, ImageDownloadedCb imageDownloadedCb);
};

#endif
