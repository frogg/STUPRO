#ifndef KRONOS_IMAGEDOWNLOADER_HPP
#define KRONOS_IMAGEDOWNLOADER_HPP

#include <qlist.h>
#include <qmap.h>
#include <qstring.h>
#include <qthreadpool.h>
#include <Utils/TileDownload/ImageLayerDescription.hpp>
#include <Utils/TileDownload/ImageTile.hpp>
#include <functional>


class ImageDownloader {
public:
	/**
	 * Type used for the tile fetched callback.
	 */
	typedef std::function<void(ImageTile tile)> OnTileFetched;

	/**
	 * Creates a new ImageDownloader using the default configuration.
	 *
	 * @param onTileFetched the callback to call whenever a tile has finished loading.
	 */
	ImageDownloader(OnTileFetched onTileFetched);

	/**
	 * Creates a new ImageDownloader using the given configuration.
	 *
	 * @param onTileFetched the callback to call whenever a tile has finished loading.
	 * @param imageLayers   a map containing layername - layerdescription objects to be used for
	 *                      loading the tiles.
	 */
	ImageDownloader(OnTileFetched onTileFetched, QMap<QString, ImageLayerDescription> imageLayers);

	/**
	 * Fetches images of all layers at the given location.
	 * The tile fetched callback given to the constructor will be called as soon as all images are
	 * loaded.
	 *
	 * @param zoomLevel how deep to dive into the quad-tree
	 * @param tileX     horizontal position of the requested tile (westernmost tile = 0)
	 * @param tileY     vertical position of the requested tile (northernmost tile = 0)
	 */
	void fetchTile(int zoomLevel, int tileX, int tileY);

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
	void fetchTile(const QString layer, int zoomLevel, int tileX, int tileY);

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
	void fetchTile(const QList<QString> layers, int zoomLevel, int tileX, int tileY);

	/**
	 * Returns a list of available image layers.
	 *
	 * @returns a list containing the names of all available image layers as read from the config
	 *          file
	 */
	QList<QString> getAvailableLayers() const;

	/**
	 * Returns a map of available image layers and their description structs.
	 *
	 * @returns a map containing a map of all available image layers as read from the config file
	 */
	QMap<QString, ImageLayerDescription> getAvailableLayerDescriptions() const;

private:
	/**
	 * Threadpool used to fetch individual tiles in an asynchronous manner.
	 */
    QThreadPool fetchThreadPool;

	/**
	 * Callback function to be called when a requested tile was downloaded or fetched from the file
	 * system.
	 */
	OnTileFetched onTileFetched;

	/**
	 * An object holding all configuration information.
	 */
	QMap<QString, ImageLayerDescription> imageLayers;
};

#endif
