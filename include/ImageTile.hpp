#ifndef KRONOS_IMAGETILE_HPP
#define KRONOS_IMAGETILE_HPP

#include <QImage>
#include <QString>
#include <QList>
#include <QMap>

class ImageTile {
public:
  /**
   * Create a new ImageTile.
   */
  ImageTile();

  /**
   * Create a new ImageTile
   * @param configuration The configuration of this downloader in JSON notation
   */
  ImageTile(const QMap<QString, QImage> &layers, int zoomLevel, int tileX, int tileY);
	~ImageTile();

  /**
   * Get the map of layers saved in this image tile.
   * @return A map of layer identifiers and corresponding images
   */
  QMap<QString, QImage>& getLayers();

  /**
   * Set the map of layers saved in this image tile.
   * @param layers A map of layer identifiers and corresponding images
   */
  void setLayers(const QMap<QString, QImage> &layers);

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
  QMap<QString, QImage> layers;
  int zoomLevel;
  int tileX;
  int tileY;
};

#endif
