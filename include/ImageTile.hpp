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
  QMap<QString, QImage> &getLayers();

  /**
   * Get the zoom level of this image tile.
   * @return The zoom level of this image tile
   */
  int getZoomLevel();

  /**
   * Get the horizontal coordinate of this image tile in this zoom level.
   * @return The horizontal position of this image tile
   */
  int getTileX();

  /**
   * Get the vertical coordinate of this image tile in this zoom level.
   * @return The vertical position of this image tile
   */
  int getTileY();

private:
  QMap<QString, QImage> layers;
  int zoomLevel;
  int tileX;
  int tileY;

};

#endif
