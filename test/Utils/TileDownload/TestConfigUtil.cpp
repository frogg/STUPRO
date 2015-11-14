#include "TestConfigUtil.hpp"

#include <cppunit/TestAssert.h>
#include <qmap.h>
#include <qstring.h>
#include <Utils/TileDownload/ConfigUtil.hpp>
#include <Utils/TileDownload/ImageLayerDescription.hpp>
#include <string>

void TestConfigUtil::testLoadConfig() {
	QMap<QString, ImageLayerDescription> layers;

	try {
		layers = ConfigUtil::loadConfigFile("./res/layers.json");
	} catch (FileOpenException e) {
		CPPUNIT_FAIL(e.what());
	}

	CPPUNIT_ASSERT_EQUAL(2, layers.size());

	ImageLayerDescription satelliteImagery = layers.value("satelliteImagery");
	CPPUNIT_ASSERT_EQUAL(
		std::string("http://worldwind25.arc.nasa.gov/wms?service=WMS&"
								"request=GetMap&version=1.3.0&crs=CRS:84&styles=&transparent=FALSE"),
		satelliteImagery.getBaseUrl().toStdString()
	);
	CPPUNIT_ASSERT_EQUAL(std::string("image/jpeg"), satelliteImagery.getMimeType().toStdString());
	CPPUNIT_ASSERT_EQUAL(512, satelliteImagery.getTileSize());

	ImageLayerDescription heightmap = layers.value("heightmap");
	CPPUNIT_ASSERT_EQUAL(
		std::string("http://worldwind26.arc.nasa.gov/wms?service=WMS&"
								"request=GetMap&version=1.3.0&crs=CRS:84&styles=&transparent=FALSE"),
		heightmap.getBaseUrl().toStdString()
	);
	CPPUNIT_ASSERT_EQUAL(std::string("application/bil16"), heightmap.getMimeType().toStdString());
	CPPUNIT_ASSERT_EQUAL(512, heightmap.getTileSize());
}

void TestConfigUtil::testLayerSteps() {
	QMap<QString, ImageLayerDescription> layers;

	try {
		layers = ConfigUtil::loadConfigFile("./res/layers.json");
	} catch (FileOpenException e) {
		CPPUNIT_FAIL(e.what());
	}

	ImageLayerDescription satelliteImagery = layers.value("satelliteImagery");

	CPPUNIT_ASSERT_EQUAL(2, satelliteImagery.getLayerSteps().size());
	CPPUNIT_ASSERT_EQUAL(0, satelliteImagery.getLayerSteps().at(0).minZoomLevel);
	CPPUNIT_ASSERT_EQUAL(
		std::string("BlueMarble-200405"),
		satelliteImagery.getLayerSteps().at(0).layers.toStdString()
	);
	CPPUNIT_ASSERT_EQUAL(8, satelliteImagery.getLayerSteps().at(1).minZoomLevel);
	CPPUNIT_ASSERT_EQUAL(
		std::string("BlueMarble-200405,esat"),
		satelliteImagery.getLayerSteps().at(1).layers.toStdString()
	);

	ImageLayerDescription heightmap = layers.value("heightmap");

	CPPUNIT_ASSERT_EQUAL(3, heightmap.getLayerSteps().size());
	CPPUNIT_ASSERT_EQUAL(0, heightmap.getLayerSteps().at(0).minZoomLevel);
	CPPUNIT_ASSERT_EQUAL(
		std::string("NASA_SRTM30_900m_Tiled"),
		heightmap.getLayerSteps().at(0).layers.toStdString()
	);
	CPPUNIT_ASSERT_EQUAL(8, heightmap.getLayerSteps().at(2).minZoomLevel);
	CPPUNIT_ASSERT_EQUAL(
		std::string("NASA_SRTM30_900m_Tiled,aster_v2,USGS-NED"),
		heightmap.getLayerSteps().at(2).layers.toStdString()
	);
}
