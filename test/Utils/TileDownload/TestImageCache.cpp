#include "TestImageCache.hpp"

#include <cppunit/TestAssert.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qglobal.h>
#include <qimage.h>
#include <qimagereader.h>
#include <qrgb.h>
#include <Utils/TileDownload/ImageCache.hpp>
#include <Utils/TileDownload/MetaImage.hpp>
#include <string>

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
					image.setPixel(x, y, qRgb(0xff, 0x00, 0xff));
				} else {
					image.setPixel(x, y, qRgb(0xff, 0xff, 0xff));
				}
			} else {
				if (x <= 256) {
					image.setPixel(x, y, qRgb(0xff, 0xff, 0xff));
				} else {
					image.setPixel(x, y, qRgb(0xff, 0x00, 0xff));
				}
			}
		}
	}

	ImageCache::getInstance().cacheImage(MetaImage(image, 1, 42),
										 QString("test-layer"), 2, 1, 3);

	QImageReader reader("cache/test-layer/tile_2_3_1.png");
	reader.setFormat("png");

	CPPUNIT_ASSERT_EQUAL(std::string("1,42"), reader.text("kronos-meta").toStdString());

	QImage readImage(512, 512, QImage::Format_RGB32);
	if (reader.read(&readImage)) {
		CPPUNIT_ASSERT_EQUAL(qRgb(0xff, 0x00, 0xff), image.pixel(128, 128));
		CPPUNIT_ASSERT_EQUAL(qRgb(0xff, 0xff, 0xff), image.pixel(128, 384));
		CPPUNIT_ASSERT_EQUAL(qRgb(0xff, 0x00, 0xff), image.pixel(384, 384));
		CPPUNIT_ASSERT_EQUAL(qRgb(0xff, 0xff, 0xff), image.pixel(384, 128));
	} else {
		CPPUNIT_FAIL("Could not read the cached image.");
	}
}

void TestImageCache::testCacheRetrieval() {
	/* Test the method that checks whether an image has been cached yet */
	CPPUNIT_ASSERT(!ImageCache::getInstance().isImageCached(QString("test-layer"), 8, 3, 7));

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
	CPPUNIT_ASSERT_EQUAL((short) 1, retrievedImage.getMinimumHeight());
	CPPUNIT_ASSERT_EQUAL((short) 42, retrievedImage.getMaximumHeight());
	CPPUNIT_ASSERT_EQUAL(512, retrievedImage.getImage().width());
	CPPUNIT_ASSERT_EQUAL(512, retrievedImage.getImage().height());
}

void TestImageCache::testClearCache() {
	QImage image(512, 512, QImage::Format_RGB32);
	ImageCache::getInstance().cacheImage(MetaImage(image, 1, 42),
										 QString("layer-to-clear"), 2, 1, 3);
	CPPUNIT_ASSERT(QDir("cache/layer-to-clear").exists());

	ImageCache::getInstance().clearCache("layer-to-clear");
	CPPUNIT_ASSERT(QDir("cache").exists());
	CPPUNIT_ASSERT(!QDir("cache/layer-to-clear").exists());
}
