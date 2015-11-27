#include <gtest/gtest.h>
#include <qimage.h>
#include <Utils/TileDownload/MetaImage.hpp>

TEST(TestMetaData, metaData) {
	QImage image(512, 512, QImage::Format_RGB32);
	MetaImage metaImageOne(image, 10, 42);

	EXPECT_TRUE(metaImageOne.hasMetaData());
	EXPECT_EQ((short) 10, metaImageOne.getMinimumHeight());
	EXPECT_EQ((short) 42, metaImageOne.getMaximumHeight());

	MetaImage metaImageTwo(image);
	EXPECT_FALSE(metaImageTwo.hasMetaData());

	metaImageTwo.setMetaData(11, 43);
	EXPECT_TRUE(metaImageTwo.hasMetaData());

	EXPECT_EQ((short) 11, metaImageTwo.getMinimumHeight());
	EXPECT_EQ((short) 43, metaImageTwo.getMaximumHeight());
}

TEST(TestMetaImage, image) {
	QImage image(512, 512, QImage::Format_RGB32);
	MetaImage metaImage(image, 1, 2);

	EXPECT_EQ(512, metaImage.getImage().width());
	EXPECT_EQ(512, metaImage.getImage().height());
}
