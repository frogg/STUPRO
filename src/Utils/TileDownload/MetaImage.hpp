#ifndef KRONOS_METAIMAGE_HPP
#define KRONOS_METAIMAGE_HPP

#include <qimage.h>

class MetaImage {
public:
  /**
   * Create a new MetaImage that will hold both the image aswell as meta data.
   * @param image The actual image data
   * @param minimumHeight The minimum height of this tile
   * @param maximumHeight The maximum height of this tile
   */
  MetaImage(const QImage &image, short minimumHeight, short maximumHeight);

  /**
   * Create a new MetaImage without meta data.
   * @param image The image data
   */
  MetaImage(const QImage &image);

  MetaImage();
	~MetaImage();

  /**
   * Get the actual image stored in this MetaImage.
   * @return The image of this MetaImage
   */
  const QImage &getImage();

  /**
   * Set the actual image stored in this MetaImage.
   * @param image The new image to be stored
   */
  void setImage(const QImage &image);

  /**
   * Get the minimum height from the meta data.
   * @return The minimum height of this MetaImage
   */
  short getMinimumHeight();

  /**
   * Get the maximum height from the meta data.
   * @return The maximum height of this MetaImage
   */
  short getMaximumHeight();

  /**
   * Set the meta data stored in this MetaImage.
   * @param minimumHeight The new minimum height
   * @param maximumHeight The new maximum height
   */
  void setMetaData(short minimumHeight, short maximumHeight);

  /**
   * Get whether this MetaImage has meta data attached.
   * @return True if there is meta data available, false otherwise
   */
  bool hasMetaData();

private:
  QImage image;
  short minimumHeight;
  short maximumHeight;
  bool metaDataAttached = false;
};

#endif
