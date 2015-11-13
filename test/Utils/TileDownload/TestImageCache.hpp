#ifndef KRONOS_TEST_TESTIMAGECACHE_HPP
#define KRONOS_TEST_TESTIMAGECACHE_HPP

#include <cppunit/extensions/AutoRegisterSuite.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestNamer.h>
#include <cppunit/TestCaller.h>
#include <qdir.h>
#include <qstring.h>

class TestImageCache : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(TestImageCache);
	CPPUNIT_TEST(testDirectorySetup);
	CPPUNIT_TEST(testCacheImage);
	CPPUNIT_TEST(testCacheRetrieval);
	CPPUNIT_TEST(testClearCache);
	CPPUNIT_TEST_SUITE_END();

public:
	void testDirectorySetup();
	void testCacheImage();
	void testCacheRetrieval();
	void testClearCache();

	/**
	 * Recursively remove a directory since this functionality is not available
	 * in Qt 4.
	 * @param dirName The name of the directory to be removed
	 * @return The result of the operation
	 */
	static bool removeDir(const QString &dirName);

	void setUp() {
		TestImageCache::removeDir(QDir("cache").absolutePath());
	}

	void tearDown() {
		TestImageCache::removeDir(QDir("cache").absolutePath());
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestImageCache);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestImageCache, TestImageCache);

#endif
