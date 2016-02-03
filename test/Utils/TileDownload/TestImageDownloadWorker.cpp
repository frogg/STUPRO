#include <gtest/gtest.h>
#include <qurl.h>
#include <Utils/TileDownload/ImageDownloadWorker.hpp>
#include <Utils/TileDownload/MetaImage.hpp>
#include <iostream>
#include <future>

TEST(TestImageDownloadWorker, Download) {
	QUrl imgUrl("http://worldwind25.arc.nasa.gov/wms?service=WMS&request=GetMap&version=1.3.0&crs=CRS:84&layers=esat&styles=&transparent=FALSE&format=image/jpeg&width=512&height=512&bbox=-90,-180,-89.9973,-179.997");

	ImageDownloadWorker worker("test", imgUrl, 512, 512);
	std::future<MetaImage> future = worker.getFuture();

	MetaImage img = future.get();

	EXPECT_EQ(512, img.getImage().width());
	EXPECT_EQ(512, img.getImage().height());
}

TEST(TestImageDownloadWorker, AbortDownload) {
	// url to an image covering a huge area that should take forever to load
	QUrl imgUrl("http://worldwind25.arc.nasa.gov/wms?service=WMS&request=GetMap&version=1.3.0&crs=CRS:84&layers=esat&styles=&transparent=FALSE&format=image/jpeg&width=512&height=512&bbox=-90,-180,90.0,180.0");

	ImageDownloadWorker worker("test", imgUrl, 512, 512);
	std::future<MetaImage> future = worker.getFuture();

	// wait until the worker has started the download
	while (!worker.isRunning());

	worker.abortDownload();

	EXPECT_THROW(future.get(), DownloadAbortedException);
}

TEST(TestImageDownloadWorker, AbortDownloadBeforeStart) {
	// url to an image covering a huge area that should take forever to load
	QUrl imgUrl("http://worldwind25.arc.nasa.gov/wms?service=WMS&request=GetMap&version=1.3.0&crs=CRS:84&layers=esat&styles=&transparent=FALSE&format=image/jpeg&width=512&height=512&bbox=-90,-180,90.0,180.0");

	ImageDownloadWorker worker("test", imgUrl, 512, 512);
	std::future<MetaImage> future = worker.getFuture();

	// don't wait until the worker has started the download (this may be non-deterministic)
	worker.abortDownload();

	EXPECT_THROW(future.get(), DownloadAbortedException);
}
