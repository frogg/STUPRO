#ifndef KRONOS_IMAGEDOWNLOADER_HPP
#define KRONOS_IMAGEDOWNLOADER_HPP

#include <qlist.h>
#include <qmap.h>
#include <qstring.h>
#include <QSet>
#include <qthreadpool.h>
#include <Utils/Misc/KronosLogger.hpp>
#include <Utils/TileDownload/ImageLayerDescription.hpp>
#include <Utils/TileDownload/ImageTile.hpp>
#include <Utils/TileDownload/ImageTileFetcher.hpp>
#include <Utils/TileDownload/ConfigUtil.hpp>

class ImageDownloader {
public:
	/**
	 * Type used for the tile fetched callback.
	 */
	typedef ImageTileFetcher::OnTileFetched OnTileFetched;

	/**
	 * Type used for the tile fetch failed callback.
	 */
	typedef ImageTileFetcher::OnTileFetchFailed OnTileFetchFailed;

	/**
	 * Creates a new ImageDownloader using the default callback for aborted downloads.
	 *
	 * @param onTileFetched the callback to call whenever a tile has finished loading.
	 * @param imageLayers       a map containing layername - layerdescription objects to be used for
	 *                          loading the tiles.
	 */
	ImageDownloader(OnTileFetched onTileFetched,
	                QMap<QString, ImageLayerDescription> imageLayers = ConfigUtil::loadConfigFile("./res/layers.json"));

	/**
	 * Creates a new ImageDownloader using different callbacks for finished downloads and aborted ones.
	 *
	 * @param onTileFetched the callback to call whenever a tile has finished loading.
	 * @param onTileFetchFailed the callback to call when a tile download was aborted or failed
	 * @param imageLayers       a map containing layername - layerdescription objects to be used for
	 *                          loading the tiles.
	 */
	ImageDownloader(OnTileFetched onTileFetched,
	                OnTileFetchFailed onTileFetchFailed,
	                QMap<QString, ImageLayerDescription> imageLayers = ConfigUtil::loadConfigFile("./res/layers.json"));

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
	 * Aborts all downloads, causing the onTileFetchFailed callback to be called instead of the
	 * onTileFetched callback.
	 */
	void abortAllDownloads();

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
	QSet<ImageTileFetcher*> activeFetchers;

	/**
	 * Callback function to be called when a requested tile was downloaded or fetched from the file
	 * system.
	 */
	OnTileFetched onTileFetched;

	/**
	 * Callback function to be called when the download of a requested tile was aborted or failed.
	 */
	OnTileFetchFailed onTileFetchFailed;

	/**
	 * An object holding all configuration information.
	 */
	QMap<QString, ImageLayerDescription> imageLayers;
};

#endif
