#include <gtest/gtest.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qglobal.h>
#include <qimage.h>
#include <qimagereader.h>
#include <qrgb.h>
#include <Utils/TileDownload/ImageCache.hpp>
#include <Utils/TileDownload/MetaImage.hpp>
#include <string>

class TestImageCache : public ::testing::Test {
public:
	static bool removeDir(const QString& dirName) {
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
	};

	void SetUp() {
		TestImageCache::removeDir(QDir("cache").absolutePath());
	};

	void TearDown() {
		TestImageCache::removeDir(QDir("cache").absolutePath());
	};
};

TEST_F(TestImageCache, DirectorySetup) {
	EXPECT_FALSE(QDir("cache").exists());
	ImageCache::getInstance();
	EXPECT_TRUE(QDir("cache").exists());
}

TEST_F(TestImageCache, CacheImage) {
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

	EXPECT_EQ(std::string("1,42"), reader.text("kronos-meta").toStdString());

	QImage readImage(512, 512, QImage::Format_RGB32);
	if (reader.read(&readImage)) {
		EXPECT_EQ(qRgb(0xff, 0x00, 0xff), image.pixel(128, 128));
		EXPECT_EQ(qRgb(0xff, 0xff, 0xff), image.pixel(128, 384));
		EXPECT_EQ(qRgb(0xff, 0x00, 0xff), image.pixel(384, 384));
		EXPECT_EQ(qRgb(0xff, 0xff, 0xff), image.pixel(384, 128));
	} else {
		FAIL() << "Could not read the cached image.";
	}
}

TEST_F(TestImageCache, CacheRetrieval) {
	/* Test the method that checks whether an image has been cached yet */
	EXPECT_FALSE(ImageCache::getInstance().isImageCached(QString("test-layer"), 8, 3, 7));

	QImage image(512, 512, QImage::Format_RGB32);
	ImageCache::getInstance().cacheImage(MetaImage(image, 1, 42),
	                                     QString("test-layer"), 8, 3, 7);

	EXPECT_TRUE(ImageCache::getInstance().isImageCached(
	                QString("test-layer"), 8, 3, 7
	            ));

	/* Test the actual retrieval method */
	EXPECT_THROW(ImageCache::getInstance()
	             .getCachedImage(QString("non-existent-layer"), 8, 3, 7),
	             ImageNotCachedException);

	MetaImage retrievedImage = ImageCache::getInstance()
	                           .getCachedImage(QString("test-layer"), 8, 3, 7);

	EXPECT_TRUE(retrievedImage.hasMetaData());
	EXPECT_EQ((short) 1, retrievedImage.getMinimumHeight());
	EXPECT_EQ((short) 42, retrievedImage.getMaximumHeight());
	EXPECT_EQ(512, retrievedImage.getImage().width());
	EXPECT_EQ(512, retrievedImage.getImage().height());
}

TEST_F(TestImageCache, ClearCache) {
	QImage image(512, 512, QImage::Format_RGB32);
	ImageCache::getInstance().cacheImage(MetaImage(image, 1, 42),
	                                     QString("layer-to-clear"), 2, 1, 3);
	EXPECT_TRUE(QDir("cache/layer-to-clear").exists());

	ImageCache::getInstance().clearCache("layer-to-clear");
	EXPECT_TRUE(QDir("cache").exists());
	EXPECT_FALSE(QDir("cache/layer-to-clear").exists());
}

TEST_F(TestImageCache, DeleteCachedImage) {
	QImage image(512, 512, QImage::Format_RGB32);
	ImageCache::getInstance().cacheImage(MetaImage(image, 1, 42),
	                                     QString("layer"), 2, 1, 3);
	ASSERT_TRUE(ImageCache::getInstance().isImageCached(
	                QString("layer"), 2, 1, 3
	            ));

	EXPECT_THROW(ImageCache::getInstance().deleteCachedImage(QString("layer"), 42, 1, 3),
	             ImageNotCachedException);

	ImageCache::getInstance().deleteCachedImage(QString("layer"), 2, 1, 3);
	EXPECT_TRUE(QDir("cache").exists());
	EXPECT_FALSE(ImageCache::getInstance().isImageCached(
	                 QString("layer"), 2, 1, 3
	             ));
}
