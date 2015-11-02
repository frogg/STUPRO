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

	CPPUNIT_ASSERT_NO_THROW(downloader.getTile(15, 0, 0));

	ImageTile tile = future.get();

	CPPUNIT_ASSERT_EQUAL(0, tile.getZoomLevel());
	CPPUNIT_ASSERT_EQUAL(0, tile.getTileX());
	CPPUNIT_ASSERT_EQUAL(0, tile.getTileY());

	CPPUNIT_ASSERT_EQUAL(1, tile.getLayers().size());

	QString layerName = downloader.getAvailableLayers()[0];
	MetaImage metaImage = tile.getLayers()[layerName];
	CPPUNIT_ASSERT_EQUAL((short)0, metaImage.getMinimumHeight());
	CPPUNIT_ASSERT_EQUAL((short)0, metaImage.getMaximumHeight());

	QImage image = metaImage.getImage();
	CPPUNIT_ASSERT_EQUAL(512, image.width());
	CPPUNIT_ASSERT_EQUAL(512, image.height());

	CPPUNIT_ASSERT_THROW(downloader.getTile(layerName, -1, 0, 0), InvalidTileZoomException);
	CPPUNIT_ASSERT_THROW(downloader.getTile(layerName, 16, 0, 0), InvalidTileZoomException);

	for (int zoom = 0; zoom < 16; zoom++) {
		CPPUNIT_ASSERT_THROW(downloader.getTile(layerName, zoom, -1, 0), InvalidTilePositionException);
		CPPUNIT_ASSERT_THROW(downloader.getTile(layerName, zoom, 8 << zoom, 0),
							 InvalidTilePositionException);
		CPPUNIT_ASSERT_THROW(downloader.getTile(layerName, zoom, 0, -1), InvalidTilePositionException);
		CPPUNIT_ASSERT_THROW(downloader.getTile(layerName, zoom, 0, 4 << zoom),
							 InvalidTilePositionException);
	}

	CPPUNIT_ASSERT_THROW(downloader.getTile("non-existing layer", 0, 0, 0), InvalidLayerException);
}
