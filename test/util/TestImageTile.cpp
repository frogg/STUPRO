#include "TestImageTile.hpp"

#include <QDebug>

int TestImageTile::testMetaData()
{
	int testResult = 0;

	QMap<QString, QImage> layers;
	ImageTile tile(layers, 10, 2, 3);

	testResult += static_cast<int>(tile.getZoomLevel() == 10);
	testResult += static_cast<int>(tile.getTileX() == 2);
	testResult += static_cast<int>(tile.getTileY() == 3);

	return static_cast<int>(testResult == 3);
}

int TestImageTile::testLayers()
{
	int testResult = 0;

	QMap<QString, QImage> layers;
	QImage imageOne(512, 512, QImage::Format_RGB32);
	QImage imageTwo(512, 512, QImage::Format_RGB32);
	layers.insert("layerOne", imageOne);
	layers.insert("layerTwo", imageTwo);
	ImageTile tile(layers, 10, 2, 3);

	testResult += static_cast<int>(tile.getLayers().size() == 2);
	testResult += static_cast<int>(tile.getLayers().contains("layerOne"));
	testResult += static_cast<int>(tile.getLayers().contains("layerTwo"));
	testResult += static_cast<int>(tile.getLayers().value("layerOne").width() == 512
									&& tile.getLayers().value("layerOne").height() == 512);
	testResult += static_cast<int>(tile.getLayers().value("layerTwo").width() == 512
									&& tile.getLayers().value("layerTwo").height() == 512);

	return static_cast<int>(testResult == 5);
}

int main(int argc, char* argv[]) {
	TestImageTile testImageTile;
	int testResult = 0;

	testResult += testImageTile.testMetaData();
	testResult += testImageTile.testLayers();

	return static_cast<int>(!(testResult == 2));
}
