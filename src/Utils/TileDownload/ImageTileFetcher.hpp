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
	/**
	 * Creates a new ImageFetcher that loads the tile with the given parameters.
	 * Executes the callback with the resulting ImageTile when done.
	 *
	 * @param layers        the layers to get the images from
	 * @param zoomLevel     how deep to dive into the quad-tree
	 * @param tileX         horizontal position of the requested tile (westernmost tile = 0)
	 * @param tileY         vertical position of the requested tile (northernmost tile = 0)
	 * @param onTileFetched function to call when the tile is loaded
	 */
	ImageTileFetcher(QMap<QString, ImageLayerDescription> availableLayers,
			QList<QString> requestedLayers, int zoomLevel, int tileX, int tileY,
			ImageDownloader::OnTileFetched onTileFetched);

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
	ImageDownloader::OnTileFetched onTileFetched;

	/**
	 * Returns the URL at which the image with of the given layer can be found.
	 *
	 * @param layer the layer from which to get the image
	 *
	 * @returns a URL pointing to the image of the requested layer
	 */
	QUrl buildTileDownloadUrl(const QString layer) const;

	/**
	 * Checks if all layers in the given list are in the available layers list.
	 *
	 * @param layers a list containing layer names
	 */
	static void validateLayersAvailable(const QList<QString> availableLayers,
			const QList<QString> requestedLayers);
};

#endif
