#include <Utils/TileDownload/ImageCache.hpp>
#include <Utils/TileDownload/ConfigUtil.hpp>

#include <QImage>
#include <QDir>
#include <QFileInfo>
#include <QImageWriter>
#include <QImageReader>

const QString ImageCache::IMAGE_FILE_EXTENSION = QString("png");
const QString ImageCache::CACHE_DIRECTORY_PATH = QString("cache");
const QString ImageCache::LAYER_DIRECTORY_PATH = QString(
	ImageCache::CACHE_DIRECTORY_PATH + "/%1"
);
const QString ImageCache::IMAGE_TILE_PATH = QString(
	ImageCache::LAYER_DIRECTORY_PATH + "/tile_%2_%3_%4." + ImageCache::IMAGE_FILE_EXTENSION
);

ImageCache &ImageCache::getInstance() {
	static ImageCache instance;
	return instance;
}

ImageCache::ImageCache() {
	/* Test whether a cache directory structure is already present */
	QDir cacheDirectory(ImageCache::CACHE_DIRECTORY_PATH);
	if (!cacheDirectory.exists()) {
		/* Create a new cache directory */
		cacheDirectory.mkpath(".");
	}
}

const void ImageCache::cacheImage(MetaImage image, QString layer, int zoomLevel, int tileX,
								  int tileY) {
	/* If necessary, create the cache directory of the specified layer */
	QDir layerDirectory(ImageCache::LAYER_DIRECTORY_PATH.arg(layer));
	if (!layerDirectory.exists()) {
		layerDirectory.mkpath(".");
	}

	/* Create a new writer putting together the file name of the image */
	QImageWriter writer(ImageCache::IMAGE_TILE_PATH
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

const void ImageCache::clearCache(QString layer) {
	QDir layerDirectory(ImageCache::LAYER_DIRECTORY_PATH.arg(layer));
	if (layerDirectory.exists()) {
		ImageCache::removeDirectory(layerDirectory.absolutePath());
	}
}

const bool ImageCache::isImageCached(QString layer, int zoomLevel, int tileX, int tileY) {
	QFileInfo imageFile(ImageCache::IMAGE_TILE_PATH
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
	QString filename = ImageCache::IMAGE_TILE_PATH
					   .arg(layer).arg(zoomLevel).arg(tileY).arg(tileX);
	QImageReader reader(filename);
	reader.setFormat(ImageCache::IMAGE_FILE_EXTENSION.toStdString().c_str());

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

bool ImageCache::removeDirectory(const QString &path) {
	bool result = true;
	QDir dir(path);

	if (dir.exists(path)) {
		Q_FOREACH (QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot
				   | QDir::System | QDir::Hidden  | QDir::AllDirs
				   | QDir::Files, QDir::DirsFirst)) {
			if (info.isDir()) {
				result = removeDirectory(info.absoluteFilePath());
			} else {
				result = QFile::remove(info.absoluteFilePath());
			}

			if (!result) {
				return result;
			}
		}
		result = dir.rmdir(path);
	}
	return result;
}
