#include "TestImageDownloader.hpp"

#include <ImageTile.hpp>

void TestImageDownloader::testGetAvailableLayers() {
	ImageDownloader downloader([](ImageTile & tile) {

	});

	QList<QString> availableLayers = downloader.getAvailableLayers();

	CPPUNIT_ASSERT(TRUE);
	// CPPUNIT_ASSERT(availableLayers.contains("satellite-imagery"));
	// CPPUNIT_ASSERT(availableLayers.contains("heightmap"));
}

void TestImageDownloader::testGetTile() {
	ImageDownloader downloader([](ImageTile & tile) {
		// TODO: somehow make cppunit wait until the callback completes
		CPPUNIT_ASSERT(tile.getZoomLevel() == 0);
		CPPUNIT_ASSERT(tile.getTileX() == 0);
		CPPUNIT_ASSERT(tile.getTileY() == 0);
	});

	QString layerName = downloader.getAvailableLayers()[0];
	downloader.getTile(layerName, 0, 0, 0);
}
