#include "TestImageDownloader.hpp"

#include <QApplication>
#include <ImageTile.hpp>
#include <iostream>
#include <future>

void TestImageDownloader::testGetAvailableLayers() {
	ImageDownloader downloader([](ImageTile & tile) {

	});

	QList<QString> availableLayers = downloader.getAvailableLayers();

	CPPUNIT_ASSERT(availableLayers.contains("satellite-imagery"));
	CPPUNIT_ASSERT(availableLayers.contains("heightmap"));
}

void TestImageDownloader::testGetTile() {
	std::promise<ImageTile> promise;
	std::future<ImageTile> future = promise.get_future();

	ImageDownloader downloader([&](ImageTile &tile) {
		promise.set_value(tile);
	});

	QString layerName = downloader.getAvailableLayers()[0];
	downloader.getTile(layerName, 0, 0, 0);

	future.wait();
	ImageTile tile = future.get();

	CPPUNIT_ASSERT(tile.getZoomLevel() == 0);
	CPPUNIT_ASSERT(tile.getTileX() == 0);
	CPPUNIT_ASSERT(tile.getTileY() == 0);
}
