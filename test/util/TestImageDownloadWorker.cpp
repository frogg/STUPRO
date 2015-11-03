#include "TestImageDownloadWorker.hpp"

#include "../../include/DebugLogger.hpp"

#include <MetaImage.hpp>

#include <future>
#include <QUrl>

void TestImageDownloadWorker::testDownload() {
	QUrl imgUrl("http://worldwind25.arc.nasa.gov/wms?service=WMS&request=GetMap&version=1.3.0&crs=CRS:84&layers=esat&styles=&transparent=FALSE&format=image/jpeg&width=512&height=512&bbox=-90,-180,-89.9973,-179.997");

	ImageDownloadWorker worker("test", imgUrl, 512, 512);
	std::future<MetaImage> future = worker.getFuture();

	MetaImage img = future.get();

	CPPUNIT_ASSERT(img.getImage().width() == 512);
}
