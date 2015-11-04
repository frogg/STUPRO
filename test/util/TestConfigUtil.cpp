#include "TestConfigUtil.hpp"

#include <ImageLayerDescription.hpp>
#include <QDebug>
#include <QString>
#include <QMap>

void TestConfigUtil::testLoadConfig() {
	QMap<QString, ImageLayerDescription> layers;

	try {
		layers = ConfigUtil::loadConfigFile("./src/res/layers.cfg");
	} catch (FileOpenException e) {
		CPPUNIT_FAIL(e.what());
	}

	CPPUNIT_ASSERT(layers.size() == 2);

	CPPUNIT_ASSERT(layers.value("satellite-imagery").baseUrl ==
				   "http://worldwind25.arc.nasa.gov/wms?service=WMS&request=GetMap&version=1.3.0&crs=CRS:84&layers=esat&styles=&transparent=FALSE");
	CPPUNIT_ASSERT(layers.value("satellite-imagery").format == "image/jpeg");
	CPPUNIT_ASSERT(layers.value("satellite-imagery").tileSize == 512);

	CPPUNIT_ASSERT(layers.value("heightmap").baseUrl ==
				   "http://worldwind26.arc.nasa.gov/wms?service=WMS&request=GetMap&version=1.3.0&crs=CRS:84&layers=NASA_SRTM30_900m_Tiled,aster_v2,USGS-NED&styles=");
	CPPUNIT_ASSERT(layers.value("heightmap").format == "image/jpeg");
	CPPUNIT_ASSERT(layers.value("heightmap").tileSize == 512);
}
