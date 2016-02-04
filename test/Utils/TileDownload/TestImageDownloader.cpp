#include <gtest/gtest.h>
#include <qglobal.h>
#include <qimage.h>
#include <qlist.h>
#include <qmap.h>
#include <qstring.h>
#include <Utils/TileDownload/ImageDownloader.hpp>
#include <Utils/TileDownload/ImageTile.hpp>
#include <Utils/TileDownload/MetaImage.hpp>
#include <future>

TEST(TestImageDownloader, GetAvailableLayers) {
	ASSERT_NO_THROW({
		ImageDownloader downloader([](ImageTile tile) {

		});

		QList<QString> availableLayers = downloader.getAvailableLayers();

		EXPECT_TRUE(availableLayers.contains("satelliteImagery"));
		EXPECT_TRUE(availableLayers.contains("heightmap"));
	});
}

TEST(TestImageDownloader, GetTile) {
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

	ASSERT_NO_THROW(downloader.fetchTile(zoomLevel, tileX, tileY));

	if (future.wait_for(std::chrono::seconds(20)) != std::future_status::ready) {
		FAIL() << "Timeout while waiting for ImageDownloader::fetchTile";
	}
	ImageTile tile = future.get();

	EXPECT_EQ(zoomLevel, tile.getZoomLevel());
	EXPECT_EQ(tileX, tile.getTileX());
	EXPECT_EQ(tileY, tile.getTileY());

	EXPECT_EQ(2, tile.getLayers().size());

	MetaImage metaImage = tile.getLayers()[layerName];
	// EXPECT_EQ((short)0, metaImage.getMinimumHeight());
	// EXPECT_EQ((short)0, metaImage.getMaximumHeight());

	QImage image = metaImage.getImage();
	EXPECT_EQ(512, image.width());
	EXPECT_EQ(512, image.height());

	// ASSERT_THROW(downloader.getTile(layerName, -1, 0, 0), InvalidTileZoomException);
	// ASSERT_THROW(downloader.getTile(layerName, 16, 0, 0), InvalidTileZoomException);

	// for (int zoom = 0; zoom < 16; zoom++) {
	//  ASSERT_THROW(downloader.getTile(layerName, zoom, -1, 0), InvalidTilePositionException);
	//  ASSERT_THROW(downloader.getTile(layerName, zoom, 8 << zoom, 0),
	//                       InvalidTilePositionException);
	//  ASSERT_THROW(downloader.getTile(layerName, zoom, 0, -1), InvalidTilePositionException);
	//  ASSERT_THROW(downloader.getTile(layerName, zoom, 0, 4 << zoom),
	//                       InvalidTilePositionException);
	// }
	//
	// ASSERT_THROW(downloader.getTile("non-existing layer", 0, 0, 0), InvalidLayerException);
}
