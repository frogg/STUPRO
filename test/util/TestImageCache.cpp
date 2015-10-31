#include "TestImageCache.hpp"

#include "MetaImage.hpp"

#include <QString>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>

bool TestImageCache::removeDir(const QString &dirName) {
	bool result = true;
	QDir dir(dirName);

	if (dir.exists(dirName)) {
		Q_FOREACH (QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot
				   | QDir::System | QDir::Hidden  | QDir::AllDirs
				   | QDir::Files, QDir::DirsFirst)) {
			if (info.isDir()) {
				result = removeDir(info.absoluteFilePath());
			} else {
				result = QFile::remove(info.absoluteFilePath());
			}

			if (!result) {
				return result;
			}
		}
		result = dir.rmdir(dirName);
	}
	return result;
}

void TestImageCache::testDirectorySetup() {
	CPPUNIT_ASSERT(!QDir("cache").exists());
	ImageCache::getInstance();
	CPPUNIT_ASSERT(QDir("cache").exists());
}

void TestImageCache::testCacheImage() {
	QImage image(512, 512, QImage::Format_RGB32);
	for (int x = 0; x < 512; ++x) {
		for (int y = 0; y < 512; ++y) {
			if (y <= 256) {
				if (x <= 256) {
					image.setPixel(x, y, qRgb(255, 0, 255));
				} else {
					image.setPixel(x, y, qRgb(255, 255, 255));
				}
			} else {
				if (x <= 256) {
					image.setPixel(x, y, qRgb(255, 255, 255));
				} else {
					image.setPixel(x, y, qRgb(255, 0, 255));
				}
			}
		}
	}

	ImageCache::getInstance().cacheImage(MetaImage(image, 1, 42),
										 QString("test-layer"), 2, 1, 3);

	QImageReader reader("cache/test-layer/tile_2_1_3.png");
	reader.setFormat("png");

	CPPUNIT_ASSERT(reader.text("kronos-meta") == "1,42");

	QImage readImage(512, 512, QImage::Format_RGB32);
	if (reader.read(&readImage)) {
		CPPUNIT_ASSERT(image.pixel(128, 128) == qRgb(255, 0, 255));
		CPPUNIT_ASSERT(image.pixel(128, 384) == qRgb(255, 255, 255));
		CPPUNIT_ASSERT(image.pixel(384, 384) == qRgb(255, 0, 255));
		CPPUNIT_ASSERT(image.pixel(384, 128) == qRgb(255, 255, 255));
	} else {
		CPPUNIT_FAIL("Could not read the cached image.");
	}
}

void TestImageCache::testCacheRetrieval() {
	/* Test the method that checks whether an image has been cached yet */
	CPPUNIT_ASSERT(!ImageCache::getInstance().isImageCached(
					   QString("test-layer"), 8, 3, 7
				   ));

	QImage image(512, 512, QImage::Format_RGB32);
	ImageCache::getInstance().cacheImage(MetaImage(image, 1, 42),
										 QString("test-layer"), 8, 3, 7);

	CPPUNIT_ASSERT(ImageCache::getInstance().isImageCached(
					   QString("test-layer"), 8, 3, 7
				   ));

	/* Test the actual retrieval method */
	CPPUNIT_ASSERT_THROW(ImageCache::getInstance()
						 .getCachedImage(QString("non-existent-layer"), 8, 3, 7),
						 ImageNotCachedException);

	MetaImage retrievedImage = ImageCache::getInstance()
							   .getCachedImage(QString("test-layer"), 8, 3, 7);
	CPPUNIT_ASSERT(retrievedImage.hasMetaData());
	CPPUNIT_ASSERT(retrievedImage.getMinimumHeight() == 1);
	CPPUNIT_ASSERT(retrievedImage.getMaximumHeight() == 42);
	CPPUNIT_ASSERT(retrievedImage.getImage().width() == 512);
	CPPUNIT_ASSERT(retrievedImage.getImage().height() == 512);
	CPPUNIT_ASSERT(retrievedImage.getImage().pixel(128, 128) == qRgb(0, 0, 0));
}
