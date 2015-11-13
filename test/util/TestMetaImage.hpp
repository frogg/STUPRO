#ifndef KRONOS_TEST_TESTMETAIMAGE_HPP
#define KRONOS_TEST_TESTMETAIMAGE_HPP

#include <cppunit/extensions/AutoRegisterSuite.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestNamer.h>
#include <cppunit/TestCaller.h>

class TestMetaImage : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(TestMetaImage);
	CPPUNIT_TEST(testMetaData);
	CPPUNIT_TEST(testImage);
	CPPUNIT_TEST_SUITE_END();

public:
	void testMetaData();
	void testImage();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestMetaImage);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestMetaImage, TestMetaImage);

#endif
