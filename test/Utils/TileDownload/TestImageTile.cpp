#include <gtest/gtest.h>
#include <qimage.h>
#include <qmap.h>
#include <qstring.h>
#include <Utils/TileDownload/ImageTile.hpp>
#include <Utils/TileDownload/MetaImage.hpp>

TEST(TestImageTile, MetaData) {
	QMap<QString, MetaImage> layers;
	ImageTile tile(layers, 10, 2, 3);

	EXPECT_EQ(10, tile.getZoomLevel());
	EXPECT_EQ(2, tile.getTileX());
	EXPECT_EQ(3, tile.getTileY());
}

TEST(TestImageTile, Layers) {
	QMap<QString, MetaImage> layers;
	QImage imageOne(512, 512, QImage::Format_RGB32);
	MetaImage metaImageOne(imageOne);
	QImage imageTwo(512, 512, QImage::Format_RGB32);
	MetaImage metaImageTwo(imageTwo);
	layers.insert("layerOne", metaImageOne);
	layers.insert("layerTwo", metaImageTwo);
	ImageTile tile(layers, 10, 2, 3);

	QMap<QString, MetaImage> layerTiles = tile.getLayers();

	EXPECT_EQ(2, layerTiles.size());
	EXPECT_TRUE(layerTiles.contains("layerOne"));
	EXPECT_TRUE(layerTiles.contains("layerTwo"));

	MetaImage layerOne = layerTiles["layerOne"];
	EXPECT_EQ(512, layerOne.getImage().width());
	EXPECT_EQ(512, layerOne.getImage().height());

	MetaImage layerTwo = layerTiles["layerTwo"];
	EXPECT_EQ(512, layerTwo.getImage().width());
	EXPECT_EQ(512, layerTwo.getImage().height());
}
