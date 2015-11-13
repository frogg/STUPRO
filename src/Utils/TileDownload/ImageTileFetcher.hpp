#ifndef KRONOS_IMAGETILEFETCHER_HPP
#define KRONOS_IMAGETILEFETCHER_HPP

#include <qlist.h>
#include <qmap.h>
#include <qrunnable.h>
#include <qstring.h>
#include <Utils/TileDownload/ImageDownloader.hpp>
#include <Utils/TileDownload/ImageLayerDescription.hpp>
#include <exception>
#include <string>

class QUrl;


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
		message = message.arg(zoomLevel);
		message = message.arg(xMin, xMax, xActual);
		message = message.arg(yMin, yMax, yActual);

		return message.toStdString().c_str();
	}
};

struct InvalidLayerException : public std::exception {
	QString givenLayer;
	QList<QString> availableLayers;

	InvalidLayerException(QString givenLayer, QList<QString> availableLayers)
		: givenLayer(givenLayer), availableLayers(availableLayers) { }

	const char * what() const throw() {
		QString message("The given layer wasn't recognized. Expected one of { ");
		for (int i = 0; i < availableLayers.size(); i++) {
			message += "'" + availableLayers[i] + "'";
			if (i < availableLayers.size() - 1) {
				message += ", ";
			}
		}
		message += " }. Was given '" + givenLayer + "'.";
		return message.toStdString().c_str();
	}
};


class ImageTileFetcher : public QRunnable {
public:
	static const int MIN_ZOOM_LEVEL = 0;
	static const int MAX_ZOOM_LEVEL = 15;
	static const int WIDTH_AT_MIN_ZOOM = 2;
	static const int HEIGHT_AT_MIN_ZOOM = 1;

	/**
	 * Creates a new ImageFetcher that loads the tile with the given parameters.
	 * Executes the callback with the resulting ImageTile when done.
	 *
	 * @param layers        the layers to get the images from
	 * @param zoomLevel     how deep to dive into the quad-tree
	 * @param tileX         horizontal position of the requested tile (westernmost tile = 0)
	 * @param tileY         vertical position of the requested tile (northernmost tile = 0)
	 * @param tileFetchedCb function to call when the tile is loaded
	 */
	ImageTileFetcher(QMap<QString, ImageLayerDescription> availableLayers,
			QList<QString> requestedLayers, int zoomLevel, int tileX, int tileY,
			ImageDownloader::TileFetchedCb tileFetchedCb);
	~ImageTileFetcher();

	/**
	 * Function executed by the thread pool when ready.
	 */
	void run();

private:
	QMap<QString, ImageLayerDescription> availableLayers;
	QList<QString> requestedLayers;
	int zoomLevel;
	int tileX;
	int tileY;
	ImageDownloader::TileFetchedCb tileFetchedCb;

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
	 * Returns the URL at which the image with of the given layer can be found.
	 *
	 * @param layer the layer from which to get the image
	 *
	 * @returns a URL pointing to the image of the requested layer
	 */
	QUrl buildTileDownloadUrl(const QString layer) const;

	/**
	 * Checks if the given tile location is valid for the underlying quad-tree.
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
	 * Checks if all layers in the given list are in the available layers list.
	 *
	 * @param layers a list containing layer names
	 */
	static void validateLayersAvailable(const QList<QString> availableLayers,
			const QList<QString> requestedLayers);
};

#endif
