#ifndef KRONOS_TEST_TESTIMAGECACHE_HPP
#define KRONOS_TEST_TESTIMAGECACHE_HPP

#include <cppunit/extensions/HelperMacros.h>

#include <ImageCache.hpp>

class TestImageCache : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(TestImageCache);
	CPPUNIT_TEST(testCacheImage);
	CPPUNIT_TEST_SUITE_END();

public:
	void testCacheImage();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestImageCache);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestImageCache, TestImageCache);

#endif
