#ifndef KRONOS_TEST_TESTCONFIGUTIL_HPP
#define KRONOS_TEST_TESTCONFIGUTIL_HPP

#include <cppunit/extensions/AutoRegisterSuite.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestNamer.h>
#include <cppunit/TestCaller.h>



class TestGlobe : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestGlobe);
    CPPUNIT_TEST(testResolution);
    CPPUNIT_TEST(testZoomLevel);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void testResolution();
    void testZoomLevel();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestGlobe);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestGlobe, TestGlobe);

#endif
