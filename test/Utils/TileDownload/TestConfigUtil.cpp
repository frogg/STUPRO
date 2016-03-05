#include <gtest/gtest.h>
#include <qmap.h>
#include <qstring.h>
#include <Utils/TileDownload/ConfigUtil.hpp>
#include <Utils/TileDownload/ImageLayerDescription.hpp>
#include <string>

TEST(TestConfigUtil, LoadConfig) {
	QMap<QString, ImageLayerDescription> layers;

	try {
		layers = ConfigUtil::loadConfigFile("./res/layers.json");
	} catch (FileOpenException e) {
		FAIL() << e.what();
	}

	EXPECT_EQ(3, layers.size());

	ImageLayerDescription satelliteImagery = layers.value("satelliteImagery");
	EXPECT_EQ(
	    std::string("http://localhost:3000/standard/satelliteImagery?q="),
	    satelliteImagery.getBaseUrl().toStdString()
	);
	EXPECT_EQ(std::string("image/jpeg"), satelliteImagery.getMimeType().toStdString());
	EXPECT_EQ(512, satelliteImagery.getTileSize());

	ImageLayerDescription heightmap = layers.value("heightmap");
	EXPECT_EQ(
	    std::string("http://localhost:3000/standard/heightmap?q="),
	    heightmap.getBaseUrl().toStdString()
	);
	EXPECT_EQ(std::string("application/bil16"), heightmap.getMimeType().toStdString());
	EXPECT_EQ(512, heightmap.getTileSize());
};

TEST(TestConfigUtil, LayerSteps) {
	QMap<QString, ImageLayerDescription> layers;

	try {
		layers = ConfigUtil::loadConfigFile("./res/layers.json");
	} catch (FileOpenException e) {
		FAIL() << e.what();
	}

	ImageLayerDescription satelliteImagery = layers.value("satelliteImagery");

	ASSERT_EQ(2, satelliteImagery.getLayerSteps().size());
	EXPECT_EQ(0, satelliteImagery.getLayerSteps().at(0).minZoomLevel);
	EXPECT_EQ(
	    std::string("BlueMarble-200405"),
	    satelliteImagery.getLayerSteps().at(0).layers.toStdString()
	);
	EXPECT_EQ(8, satelliteImagery.getLayerSteps().at(1).minZoomLevel);
	EXPECT_EQ(
	    std::string("BlueMarble-200405,esat"),
	    satelliteImagery.getLayerSteps().at(1).layers.toStdString()
	);

	ImageLayerDescription heightmap = layers.value("heightmap");

	ASSERT_EQ(3, heightmap.getLayerSteps().size());
	EXPECT_EQ(0, heightmap.getLayerSteps().at(0).minZoomLevel);
	EXPECT_EQ(
	    std::string("NASA_SRTM30_900m_Tiled"),
	    heightmap.getLayerSteps().at(0).layers.toStdString()
	);
	EXPECT_EQ(8, heightmap.getLayerSteps().at(2).minZoomLevel);
	EXPECT_EQ(
	    std::string("NASA_SRTM30_900m_Tiled,aster_v2,USGS-NED"),
	    heightmap.getLayerSteps().at(2).layers.toStdString()
	);
}
