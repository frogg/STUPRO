#ifndef KRONOS_TEST_TESTIMAGEDOWNLOADER_HPP
#define KRONOS_TEST_TESTIMAGEDOWNLOADER_HPP

#include <cppunit/extensions/HelperMacros.h>

#include <ImageDownloader.hpp>

class TestImageDownloader : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TestImageDownloader);
    CPPUNIT_TEST(testGetAvailableLayers);
    CPPUNIT_TEST(testGetImage);
  CPPUNIT_TEST_SUITE_END();

public:
  void testGetAvailableLayers();
  void testGetImage();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestImageDownloader);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestImageDownloader, "TestImageDownloader");

#endif
