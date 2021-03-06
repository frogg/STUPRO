#ifndef KRONOS_IMAGEDOWNLOADER_HPP
#define KRONOS_IMAGEDOWNLOADER_HPP

#include <QString>
#include <QSet>
#include <Utils/TileDownload/TileRequestWorker.hpp>
#include <memory>

class ImageDownloader {
public:
	/**
	 * Type used for the tile fetched callback.
	 */
	typedef TileRequestWorker::OnTileFetched OnTileFetched;

	/**
	 * Type used for the tile fetch failed callback.
	 */
	typedef TileRequestWorker::OnTileFetchFailed OnTileFetchFailed;

	/**
	 * Creates a new ImageDownloader using the default callback for aborted downloads.
	 *
	 * @param onTileFetched   the callback to call whenever a tile has finished loading.
	 * @param configFile      path to the JSON file from which to read the layer config
	 */
	ImageDownloader(OnTileFetched onTileFetched, QString configFile = "./res/layers.json");

	/**
	 * Creates a new ImageDownloader using the default callback for aborted downloads.
	 *
	 * @param onTileFetched   the callback to call whenever a tile has finished loading.
	 * @param requestedLayers a map containing layername - layerdescription objects to be used for
	 *                        loading the tiles.
	 * @param configFile      path to the JSON file from which to read the layer config
	 */
	ImageDownloader(OnTileFetched onTileFetched, QSet<QString> requestedLayers,
	                QString configFile = "./res/layers.json");

	/**
	 * Creates a new ImageDownloader using different callbacks for finished downloads and aborted ones.
	 *
	 * @param onTileFetched the callback to call whenever a tile has finished loading.
	 * @param onTileFetchFailed the callback to call when a tile download was aborted or failed
	 * @param configFile      path to the JSON file from which to read the layer config
	 */
	ImageDownloader(OnTileFetched onTileFetched, OnTileFetchFailed onTileFetchFailed,
	                QString configFile = "./res/layers.json");

	/**
	 * Creates a new ImageDownloader using different callbacks for finished downloads and aborted ones.
	 *
	 * @param onTileFetched the callback to call whenever a tile has finished loading.
	 * @param onTileFetchFailed the callback to call when a tile download was aborted or failed
	 * @param requestedLayers   a map containing layername - layerdescription objects to be used for
	 *                          loading the tiles.
	 * @param configFile      path to the JSON file from which to read the layer config
	 */
	ImageDownloader(OnTileFetched onTileFetched, OnTileFetchFailed onTileFetchFailed,
	                QSet<QString> requestedLayers, QString configFile = "./res/layers.json");

	/**
	 * Fetches images of all layers at the given location.
	 * The tile fetched callback given to the constructor will be called as soon as all images are
	 * loaded.
	 *
	 * @param zoomLevel how deep to dive into the quad-tree
	 * @param tileX     horizontal position of the requested tile (westernmost tile = 0)
	 * @param tileY     vertical position of the requested tile (northernmost tile = 0)
	 */
	void requestTile(int zoomLevel, int tileX, int tileY);

	/**
	 * Aborts all downloads, causing the onTileFetchFailed callback to be called instead of the
	 * onTileFetched callback.
	 */
	void abortAllRequests();

	const QSet<QString> getRequestedLayers() const;

private:
	/** Worker that handles all tile requests. */
	std::unique_ptr<TileRequestWorker> requestWorker;

	static void defaultErrorHandler(std::exception const& e);

	static QSet<QString> getAllAvailableLayers();
};

#endif
