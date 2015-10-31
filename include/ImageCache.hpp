#ifndef KRONOS_IMAGECACHE_HPP
#define KRONOS_IMAGECACHE_HPP

#include "MetaImage.hpp"

#include <QString>

class ImageCache {
public:
  /**
   * Check whether an image tile of a certain layer has already been saved.
   * @param layer Unique identifier of an existing layer
   * @param zoomLevel Zoom level down the quad tree of images
   * @param tileX Horizontal position of the requested tile
   * @param tileY Vertical position of the requested tile
   * @return True if the image has been cached, false otherwise
   */
  static const bool isImageCached(QString &layer, int zoomLevel, int tileX, int tileY);

  /**
   * Get a saved image tile from the cache.
   * @param layer Unique identifier of an existing layer
   * @param zoomLevel Zoom level down the quad tree of images
   * @param tileX Horizontal position of the requested tile
   * @param tileY Vertical position of the requested tile
   * @return A pointer to the image loaded from the cache
   */
  static const MetaImage getCachedImage(QString &layer, int zoomLevel, int tileX, int tileY);

  /**
   * Save an existing image to the cache.
   * @param image The image data of the tile to be saved
   * @param layer Unique identifier of an existing layer
   * @param zoomLevel Zoom level down the quad tree of images
   * @param tileX Horizontal position of the requested tile
   * @param tileY Vertical position of the requested tile
   */
  static const void cacheImage(MetaImage &image, QString &layer, int zoomLevel, int tileX, int tileY);
};

#endif
