#include "TestMetaImage.hpp"

#include <QDebug>

void TestMetaImage::testMetaData() {
	QImage image(512, 512, QImage::Format_RGB32);
	MetaImage metaImageOne(image, 10, 42);

	CPPUNIT_ASSERT(metaImageOne.hasMetaData());
	CPPUNIT_ASSERT_EQUAL((short) 10, metaImageOne.getMinimumHeight());
	CPPUNIT_ASSERT_EQUAL((short) 42, metaImageOne.getMaximumHeight());

	MetaImage metaImageTwo(image);
	CPPUNIT_ASSERT(!metaImageTwo.hasMetaData());

	metaImageTwo.setMetaData(11, 43);
	CPPUNIT_ASSERT(metaImageTwo.hasMetaData());

	CPPUNIT_ASSERT_EQUAL((short) 11, metaImageTwo.getMinimumHeight());
	CPPUNIT_ASSERT_EQUAL((short) 43, metaImageTwo.getMaximumHeight());
}

void TestMetaImage::testImage() {
	QImage image(512, 512, QImage::Format_RGB32);
	MetaImage metaImage(image, 1, 2);

	CPPUNIT_ASSERT_EQUAL(512, metaImage.getImage().width());
	CPPUNIT_ASSERT_EQUAL(512, metaImage.getImage().height());
}
