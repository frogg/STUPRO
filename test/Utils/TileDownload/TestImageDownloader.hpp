#ifndef KRONOS_TEST_TESTIMAGEDOWNLOADER_HPP
#define KRONOS_TEST_TESTIMAGEDOWNLOADER_HPP

#include <cppunit/extensions/AutoRegisterSuite.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestNamer.h>
#include <cppunit/TestCaller.h>

class TestImageDownloader : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(TestImageDownloader);
	CPPUNIT_TEST(testGetAvailableLayers);
	CPPUNIT_TEST(testGetTile);
	CPPUNIT_TEST_SUITE_END();

public:
	void testGetAvailableLayers();
	void testGetTile();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestImageDownloader);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestImageDownloader, "TestImageDownloader");

#endif
