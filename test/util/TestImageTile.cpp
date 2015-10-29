#include "TestImageTile.hpp"

#include <QDebug>

void TestImageTile::testMetaData()
{
	QMap<QString, QImage> layers;
	ImageTile tile(layers, 10, 2, 3);

	CPPUNIT_ASSERT(tile.getZoomLevel() == 10);
	CPPUNIT_ASSERT(tile.getTileX() == 2);
	CPPUNIT_ASSERT(tile.getTileY() == 3);
}

void TestImageTile::testLayers()
{
	QMap<QString, QImage> layers;
	QImage imageOne(512, 512, QImage::Format_RGB32);
	QImage imageTwo(512, 512, QImage::Format_RGB32);
	layers.insert("layerOne", imageOne);
	layers.insert("layerTwo", imageTwo);
	ImageTile tile(layers, 10, 2, 3);

	CPPUNIT_ASSERT(tile.getLayers().size() == 2);
	CPPUNIT_ASSERT(tile.getLayers().contains("layerOne"));
	CPPUNIT_ASSERT(tile.getLayers().contains("layerTwo"));
	CPPUNIT_ASSERT(tile.getLayers().contains("layerTwo"));
	CPPUNIT_ASSERT(tile.getLayers().value("layerOne").width() == 512
	&& tile.getLayers().value("layerOne").height() == 512);
	CPPUNIT_ASSERT(tile.getLayers().value("layerTwo").width() == 512
	&& tile.getLayers().value("layerTwo").height() == 512);
}
