#include "ImageCache.hpp"
#include "ConfigUtil.hpp"

#include <QImage>
#include <QDir>
#include <QFileInfo>
#include <QImageWriter>
#include <QImageReader>

ImageCache &ImageCache::getInstance() {
	static ImageCache instance;
	return instance;
}

ImageCache::ImageCache() {
	/* Test whether a cache directory structure is already present */
	QDir cacheDirectory("cache");
	if (!cacheDirectory.exists()) {
		/* Create a new cache directory */
		cacheDirectory.mkpath(".");
	}
}

const void ImageCache::cacheImage(MetaImage image, QString layer, int zoomLevel, int tileX,
								  int tileY) {
	/* If necessary, create the cache directory of the specified layer */
	QDir layerDirectory(QString("cache/%1").arg(layer));
	if (!layerDirectory.exists()) {
		layerDirectory.mkpath(".");
	}

	/* Create a new writer putting together the file name of the image */
	QImageWriter writer(QString("cache/%1/tile_%2_%3_%4.png")
						.arg(layer).arg(zoomLevel).arg(tileY).arg(tileX));

	/*
	 * Save the image size as meta data since Qt does not allow to easily read it
	 * later from the image file.
	 */
	writer.setText("image-size", QString("%1,%2")
				   .arg(image.getImage().width()).arg(image.getImage().height()));

	/* Write potential additional meta data */
	if (image.hasMetaData()) {
		writer.setText("kronos-meta", QString("%1,%2")
					   .arg(image.getMinimumHeight()).arg(image.getMaximumHeight()));
	}

	writer.write(image.getImage());
}

const bool ImageCache::isImageCached(QString layer, int zoomLevel, int tileX, int tileY) {
	QFileInfo imageFile(QString("cache/%1/tile_%2_%3_%4.png")
						.arg(layer).arg(zoomLevel).arg(tileY).arg(tileX));
	return imageFile.exists() && imageFile.isFile();
}

const MetaImage ImageCache::getCachedImage(QString layer, int zoomLevel, int tileX,
		int tileY) {
	if (!ImageCache::getInstance().isImageCached(layer, zoomLevel, tileX, tileY)) {
		throw ImageNotCachedException(QString("The requested image from layer %1 with"
											  " zoom level %2 and position %3/%4 has not been cached yet.")
									  .arg(layer).arg(zoomLevel).arg(tileX).arg(tileY));
	}

	/* Create a new reader putting together the file name of the image */
	QString filename = QString("cache/%1/tile_%2_%3_%4.png")
					   .arg(layer).arg(zoomLevel).arg(tileY).arg(tileX);
	QImageReader reader(filename);
	reader.setFormat("png");

	/* Create a new image with the image dimensions as read from the file meta data */
	QStringList imageSize = reader.text("image-size").split(",");
	QImage readImage(imageSize.at(0).toInt(), imageSize.at(1).toInt(), QImage::Format_RGB32);

	/* Read the image and possible meta data and return everything */
	if (reader.read(&readImage)) {
		if (reader.textKeys().contains("kronos-meta")) {
			QStringList metaData = reader.text("kronos-meta").split(",");
			return MetaImage(readImage,
							 metaData.at(0).toInt(), metaData.at(1).toInt());
		} else {
			return MetaImage(readImage);
		}
	} else {
		throw ImageNotCachedException(QString("The requested image from layer %1 with"
											  " zoom level %2 and position %3/%4 could not be read.")
									  .arg(layer).arg(zoomLevel).arg(tileX).arg(tileY));
	}
}
