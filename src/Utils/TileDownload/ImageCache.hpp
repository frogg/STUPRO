#ifndef KRONOS_IMAGECACHE_HPP
#define KRONOS_IMAGECACHE_HPP

#include <qstring.h>
#include <Utils/TileDownload/MetaImage.hpp>
#include <exception>
#include <string>

struct ImageNotCachedException : public std::exception {
	QString message;

	ImageNotCachedException(QString message) : message(message) { }

	const char* what () const throw () {
		return message.toStdString().c_str();
	}
};

class ImageCache {
public:
	/**
	 * Get the singleton instance of this class.
	 * @return The singleton instance of this class
	 */
	static ImageCache& getInstance();

	/**
	 * Check whether an image tile of a certain layer has already been saved.
	 * @param layer Unique identifier of an existing layer
	 * @param zoomLevel Zoom level down the quad tree of images
	 * @param tileX Horizontal position of the requested tile
	 * @param tileY Vertical position of the requested tile
	 * @return True if the image has been cached, false otherwise
	 */
	const bool isImageCached(QString layer, int zoomLevel, int tileX, int tileY);

	/**
	 * Get a saved image tile from the cache.
	 * @param layer Unique identifier of an existing layer
	 * @param zoomLevel Zoom level down the quad tree of images
	 * @param tileX Horizontal position of the requested tile
	 * @param tileY Vertical position of the requested tile
	 * @return A pointer to the image loaded from the cache
	 * @throws ImageNotCachedException If the requested image has not been cached yet
	 */
	const MetaImage getCachedImage(QString layer, int zoomLevel, int tileX, int tileY);

	/**
	 * Save an existing image to the cache.
	 * @param image The image data of the tile to be saved
	 * @param layer Unique identifier of an existing layer
	 * @param zoomLevel Zoom level down the quad tree of images
	 * @param tileX Horizontal position of the requested tile
	 * @param tileY Vertical position of the requested tile
	 */
	const void cacheImage(MetaImage image, QString layer, int zoomLevel, int tileX, int tileY);

	/**
	 * Delete all cached data from a specified layer.
	 * @param layer The unique identifier of the layer whose cache should be cleared
	 */
	const void clearCache(QString layer);

private:
	/*
	 * Hide some things that should not be accessed given this class uses
	 * the singleton pattern.
	 */
	ImageCache();
	ImageCache(ImageCache const&) = delete;
	void operator=(ImageCache const&) = delete;

	/**
	 * Recursively delete a directory even if it is not empty.
	 * This method is needed since Qt offers the same functionality only with
	 * version 5.
	 * @param path Path of the directory to be deleted
	 */
	static bool removeDirectory(const QString& path);

	/**
	 * The image file extension that will be used to cache images.
	 */
	static const QString IMAGE_FILE_EXTENSION;
	/**
	 * The directory where the cache's root is located.
	 */
	static const QString CACHE_DIRECTORY_PATH;
	/**
	 * The directory where a specific layer in the cache is located.
	 */
	static const QString LAYER_DIRECTORY_PATH;
	/**
	 * The full path of a specific image tile.
	 */
	static const QString IMAGE_TILE_PATH;

	/**
	 * Tag of the meta data specifying the image's resolution that will be written
	 * to the image header.
	 */
	static const QString META_TAG_IMAGE_SIZE;
	/**
	 * Tag of the meta data specifying the image's minimum and maximum height
	 * value as represented respectively by the darkest and the brightest
	 * pixel.
	 * This is only relevant for images containing heightmaps.
	 */
	static const QString META_TAG_HEIGHT_DATA;

	/**
	 * Error message used if the requested image file has not been cached yet.
	 */
	static const QString IMAGE_NOT_CACHED_MESSAGE;
	/**
	 * Error message used if the requested image file could not be read.
	 */
	static const QString IMAGE_COULD_NOT_BE_READ_MESSAGE;
};

#endif
