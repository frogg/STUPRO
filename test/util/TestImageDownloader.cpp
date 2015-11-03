#include "TestImageDownloader.hpp"

#include "../../include/DebugLogger.hpp"

#include <QApplication>
#include <ImageTile.hpp>
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
		DebugLogger::debug("dl callback");
		promise.set_value(tile);
	});

	const int zoomLevel = 10;
	const int tileX = 3000;
	const int tileY = 500;
	const QString layerName = downloader.getAvailableLayers()[1];
	DebugLogger::debug(layerName);

	CPPUNIT_ASSERT_NO_THROW(downloader.getTile(layerName, zoomLevel, tileX, tileY));

	DebugLogger::debug("waiting for future");

	ImageTile tile = future.get();

	DebugLogger::debug("future returned");

	CPPUNIT_ASSERT_EQUAL(zoomLevel, tile.getZoomLevel());
	CPPUNIT_ASSERT_EQUAL(tileX, tile.getTileX());
	CPPUNIT_ASSERT_EQUAL(tileY, tile.getTileY());

	CPPUNIT_ASSERT_EQUAL(1, tile.getLayers().size());

	MetaImage metaImage = tile.getLayers()[layerName];
	CPPUNIT_ASSERT_EQUAL((short)0, metaImage.getMinimumHeight());
	CPPUNIT_ASSERT_EQUAL((short)0, metaImage.getMaximumHeight());

	QImage image = metaImage.getImage();
	CPPUNIT_ASSERT_EQUAL(512, image.width());
	CPPUNIT_ASSERT_EQUAL(512, image.height());

	// CPPUNIT_ASSERT_THROW(downloader.getTile(layerName, -1, 0, 0), InvalidTileZoomException);
	// CPPUNIT_ASSERT_THROW(downloader.getTile(layerName, 16, 0, 0), InvalidTileZoomException);

	// for (int zoom = 0; zoom < 16; zoom++) {
	//  CPPUNIT_ASSERT_THROW(downloader.getTile(layerName, zoom, -1, 0), InvalidTilePositionException);
	//  CPPUNIT_ASSERT_THROW(downloader.getTile(layerName, zoom, 8 << zoom, 0),
	//                       InvalidTilePositionException);
	//  CPPUNIT_ASSERT_THROW(downloader.getTile(layerName, zoom, 0, -1), InvalidTilePositionException);
	//  CPPUNIT_ASSERT_THROW(downloader.getTile(layerName, zoom, 0, 4 << zoom),
	//                       InvalidTilePositionException);
	// }
	//
	// CPPUNIT_ASSERT_THROW(downloader.getTile("non-existing layer", 0, 0, 0), InvalidLayerException);
}
