#include "TestImageDownloader.hpp"

#include <QApplication>
#include <ImageTile.hpp>
#include <iostream>
#include <future>

void TestImageDownloader::testGetAvailableLayers() {
	ImageDownloader downloader([](ImageTile tile) {

	});

	QList<QString> availableLayers = downloader.getAvailableLayers();

	CPPUNIT_ASSERT(availableLayers.contains("satellite-imagery"));
	CPPUNIT_ASSERT(availableLayers.contains("heightmap"));
}

void TestImageDownloader::testGetTile() {
	std::promise<ImageTile> promise;
	std::future<ImageTile> future = promise.get_future();

	ImageDownloader downloader([&](ImageTile tile) {
		promise.set_value(tile);
	});

	QString layerName = downloader.getAvailableLayers()[0];
	downloader.getTile(layerName, 0, 0, 0);

	future.wait();
	ImageTile tile = future.get();

	CPPUNIT_ASSERT_EQUAL(0, tile.getZoomLevel());
	CPPUNIT_ASSERT_EQUAL(0, tile.getTileX());
	CPPUNIT_ASSERT_EQUAL(0, tile.getTileY());

	CPPUNIT_ASSERT_EQUAL(1, tile.getLayers().size());

	MetaImage metaImage = tile.getLayers()[layerName];
	CPPUNIT_ASSERT_EQUAL((short)0, metaImage.getMinimumHeight());
	CPPUNIT_ASSERT_EQUAL((short)0, metaImage.getMaximumHeight());

	QImage image = metaImage.getImage();
	CPPUNIT_ASSERT_EQUAL(512, image.width());
	CPPUNIT_ASSERT_EQUAL(512, image.height());
}
