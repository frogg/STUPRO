#include "TestImageTile.hpp"

#include <QDebug>

void TestImageTile::testMetaData() {
	QMap<QString, MetaImage> layers;
	ImageTile tile(layers, 10, 2, 3);

	CPPUNIT_ASSERT(tile.getZoomLevel() == 10);
	CPPUNIT_ASSERT(tile.getTileX() == 2);
	CPPUNIT_ASSERT(tile.getTileY() == 3);
}

void TestImageTile::testLayers() {
	QMap<QString, MetaImage> layers;
	QImage imageOne(512, 512, QImage::Format_RGB32);
	MetaImage metaImageOne(imageOne);
	QImage imageTwo(512, 512, QImage::Format_RGB32);
	MetaImage metaImageTwo(imageTwo);
	layers.insert("layerOne", metaImageOne);
	layers.insert("layerTwo", metaImageTwo);
	ImageTile tile(layers, 10, 2, 3);

	QMap<QString, MetaImage> layerTiles = tile.getLayers();
	CPPUNIT_ASSERT(layerTiles.size() == 2);
	CPPUNIT_ASSERT(layerTiles.contains("layerOne"));
	CPPUNIT_ASSERT(layerTiles.contains("layerTwo"));
	MetaImage layerOne = layerTiles["layerOne"];
	CPPUNIT_ASSERT(layerOne.getImage().width() == 512
				   && layerOne.getImage().height() == 512);
	MetaImage layerTwo = layerTiles["layerTwo"];
	CPPUNIT_ASSERT(layerTwo.getImage().width() == 512
				   && layerTwo.getImage().height() == 512);
}
