#include "../include/ImageCache.hpp"

#include <QImage>

const void ImageCache::cacheImage(MetaImage &image, QString &layer, int zoomLevel, int tileX,
								  int tileY) {
	// TODO: Save meta-data, e.g. with image file meta-data?

	/*QFile file("test.jpg");
	if (!file.open(QIODevice::WriteOnly))
	  return 0;
	QTextStream out(&file);
	out << image;
	file.close();
	myImage->load("out.png");*/
}
