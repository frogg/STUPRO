#ifndef KRONOS_IMAGETILEFETCHER_HPP
#define KRONOS_IMAGETILEFETCHER_HPP

#include <qlist.h>
#include <qmap.h>
#include <qrunnable.h>
#include <qstring.h>
#include <Utils/TileDownload/DownloadExceptions.hpp>
#include <Utils/TileDownload/ImageLayerDescription.hpp>
#include <Utils/TileDownload/ImageTile.hpp>
#include <string>
#include <functional>


class QUrl;

class ImageTileFetcher : public QRunnable {
public:
	/**
	 * Type used for the tile fetched callback.
	 */
	typedef std::function<void(ImageTile tile)> OnTileFetched;

	/**
	 * Type used for the tile fetch failed callback.
	 */
	typedef std::function<void(std::exception const& e)> OnTileFetchFailed;

	/**
	 * Creates a new ImageFetcher that loads the tile with the given parameters.
	 * Executes the callback with the resulting ImageTile when done.
	 *
	 * @param layers        the layers to get the images from
	 * @param zoomLevel     how deep to dive into the quad-tree
	 * @param tileX         horizontal position of the requested tile (westernmost tile = 0)
	 * @param tileY         vertical position of the requested tile (northernmost tile = 0)
	 */
	ImageTileFetcher(QMap<QString, ImageLayerDescription> availableLayers,
	                 QList<QString> requestedLayers, int zoomLevel, int tileX, int tileY);

	/**
	 * Aborts the currently running fetching process.
	 */
	void abortFetching();

	/**
	 * Function executed by the thread pool when ready.
	 */
	void run();

	/**
	 * Sets the callback to execute when a tile was sucessfully fetched.
	 */
	void setOnTileFetched(OnTileFetched onTileFetched);

	/**
	 * Sets the callback to execute when a tile failed to be fetched.
	 */
	void setOnTileFetchFailed(OnTileFetchFailed onTileFetchFailed);

private:
	QMap<QString, ImageLayerDescription> availableLayers;
	QList<QString> requestedLayers;
	int zoomLevel;
	int tileX;
	int tileY;
	bool abortRequested;
	OnTileFetched onTileFetched;
	OnTileFetchFailed onTileFetchFailed;

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
