#ifndef KRONOS_IMAGETILE_HPP
#define KRONOS_IMAGETILE_HPP

#include "MetaImage.hpp"

#include <QString>
#include <QMap>

class ImageTile {
public:
  /**
   * Create a new ImageTile.
   * @param layers A map of layer identifiers and the corresponding image
   * @param zoomLevel The zoom level of this image tile
   * @param tileX The horizontal position of this tile
   * @param tileY The vertical position of this tile
   */
  ImageTile(const QMap<QString, MetaImage*> &layers, int zoomLevel, int tileX, int tileY);
	~ImageTile();

  /**
   * Get the map of layers saved in this image tile.
   * @return A map of layer identifiers and corresponding images
   */
  QMap<QString, MetaImage*>& getLayers();

  /**
   * Set the map of layers saved in this image tile.
   * @param layers A map of layer identifiers and corresponding images
   */
  void setLayers(const QMap<QString, MetaImage*> &layers);

  /**
   * Get the zoom level of this image tile.
   * @return The zoom level of this image tile
   */
  int getZoomLevel();

  /**
   * Set the zoom level of this image tile.
   * @param zoomLevel The new zoom level of this image tile
   */
  void setZoomLevel(int zoomLevel);

  /**
   * Get the horizontal coordinate of this image tile in this zoom level.
   * @return The horizontal position of this image tile
   */
  int getTileX();

  /**
   * Set the horizontal coordinate of this image tile in this zoom level.
   * @param tileX The new horizontal position of this image tile
   */
  void setTileX(int tileX);

  /**
   * Get the vertical coordinate of this image tile in this zoom level.
   * @return The vertical position of this image tile
   */
  int getTileY();

  /**
   * Set the vertical coordinate of this image tile in this zoom level.
   * @param tileY The new vertical position of this image tile
   */
  void setTileY(int tileY);

private:
  QMap<QString, MetaImage*> layers;
  int zoomLevel;
  int tileX;
  int tileY;
};

#endif
