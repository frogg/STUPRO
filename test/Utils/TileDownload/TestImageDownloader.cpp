#include "TestImageDownloader.hpp"

#include <cppunit/TestAssert.h>
#include <qglobal.h>
#include <qimage.h>
#include <qlist.h>
#include <qmap.h>
#include <qstring.h>
#include <Utils/TileDownload/ImageDownloader.hpp>
#include <Utils/TileDownload/ImageTile.hpp>
#include <Utils/TileDownload/MetaImage.hpp>
#include <future>

void TestImageDownloader::testGetAvailableLayers() {
	CPPUNIT_ASSERT_NO_THROW({
		ImageDownloader downloader([](ImageTile tile) {

		});

		QList<QString> availableLayers = downloader.getAvailableLayers();

		CPPUNIT_ASSERT(availableLayers.contains("satelliteImagery"));
		CPPUNIT_ASSERT(availableLayers.contains("heightmap"));
	});
}

void TestImageDownloader::testGetTile() {
	std::promise<ImageTile> promise;
	std::future<ImageTile> future = promise.get_future();

	ImageDownloader downloader([&](ImageTile tile) {
		try {
			promise.set_value(tile);
		} catch (...) { }
	});

	const int zoomLevel = 10;
	const int tileX = 20;
	const int tileY = 30;
	const QString layerName = downloader.getAvailableLayers()[0];

	CPPUNIT_ASSERT_NO_THROW(downloader.getTile(zoomLevel, tileX, tileY));

	ImageTile tile = future.get();

	CPPUNIT_ASSERT_EQUAL(zoomLevel, tile.getZoomLevel());
	CPPUNIT_ASSERT_EQUAL(tileX, tile.getTileX());
	CPPUNIT_ASSERT_EQUAL(tileY, tile.getTileY());

	CPPUNIT_ASSERT_EQUAL(2, tile.getLayers().size());

	MetaImage metaImage = tile.getLayers()[layerName];
	// CPPUNIT_ASSERT_EQUAL((short)0, metaImage.getMinimumHeight());
	// CPPUNIT_ASSERT_EQUAL((short)0, metaImage.getMaximumHeight());

	QImage image = metaImage.getImage();
	CPPUNIT_ASSERT_EQUAL(512, image.width());
	CPPUNIT_ASSERT_EQUAL(512, image.height());

	// for (int x = 0; x < 8; x++) {
	//  for (int y = 0; y < 4; y++) {
	//      downloader.getTile(0, x, y);
	//  }
	// }

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
