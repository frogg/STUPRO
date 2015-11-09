#include "TestImageTile.hpp"

#include <QDebug>

void TestImageTile::testMetaData() {
	QMap<QString, MetaImage> layers;
	ImageTile tile(layers, 10, 2, 3);

	CPPUNIT_ASSERT_EQUAL(10, tile.getZoomLevel());
	CPPUNIT_ASSERT_EQUAL(2, tile.getTileX());
	CPPUNIT_ASSERT_EQUAL(3, tile.getTileY());
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

	CPPUNIT_ASSERT_EQUAL(2, layerTiles.size());
	CPPUNIT_ASSERT(layerTiles.contains("layerOne"));
	CPPUNIT_ASSERT(layerTiles.contains("layerTwo"));

	MetaImage layerOne = layerTiles["layerOne"];
	CPPUNIT_ASSERT_EQUAL(512, layerOne.getImage().width());
	CPPUNIT_ASSERT_EQUAL(512, layerOne.getImage().height());

	MetaImage layerTwo = layerTiles["layerTwo"];
	CPPUNIT_ASSERT_EQUAL(512, layerTwo.getImage().width());
	CPPUNIT_ASSERT_EQUAL(512, layerTwo.getImage().height());
}
