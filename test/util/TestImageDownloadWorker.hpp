#ifndef KRONOS_TEST_TESTIMAGEDOWNLOADWORKER_HPP
#define KRONOS_TEST_TESTIMAGEDOWNLOADWORKER_HPP

#include <cppunit/extensions/AutoRegisterSuite.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestNamer.h>
#include <cppunit/TestCaller.h>

class TestImageDownloadWorker : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(TestImageDownloadWorker);
	CPPUNIT_TEST(testDownload);
	CPPUNIT_TEST_SUITE_END();

public:
	void testDownload();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestImageDownloadWorker);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestImageDownloadWorker, "TestImageDownloadWorker");

#endif
