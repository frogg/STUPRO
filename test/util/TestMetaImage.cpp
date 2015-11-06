#include "TestMetaImage.hpp"

#include <QDebug>

void TestMetaImage::testMetaData() {
	QImage image(512, 512, QImage::Format_RGB32);
	MetaImage metaImageOne(image, 10, 42);

	CPPUNIT_ASSERT(metaImageOne.hasMetaData());
	CPPUNIT_ASSERT(metaImageOne.getMinimumHeight() == 10);
	CPPUNIT_ASSERT(metaImageOne.getMaximumHeight() == 42);

	MetaImage metaImageTwo(image);

	CPPUNIT_ASSERT(!metaImageTwo.hasMetaData());

	metaImageTwo.setMetaData(11, 43);

	CPPUNIT_ASSERT(metaImageTwo.getMinimumHeight() == 11);
	CPPUNIT_ASSERT(metaImageTwo.getMaximumHeight() == 43);
}

void TestMetaImage::testImage() {
	QImage image(512, 512, QImage::Format_RGB32);
	MetaImage metaImage(image, 1, 2);

	CPPUNIT_ASSERT(metaImage.getImage().width() == 512);
	CPPUNIT_ASSERT(metaImage.getImage().height() == 512);
}
