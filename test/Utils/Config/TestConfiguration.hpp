#ifndef KRONOS_TEST_TESTCONFIGURATION_HPP
#define KRONOS_TEST_TESTCONFIGURATION_HPP

#include <cppunit/extensions/AutoRegisterSuite.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestNamer.h>
#include <cppunit/TestCaller.h>

class TestConfiguration : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(TestConfiguration);
	CPPUNIT_TEST(testValidCalls);
	CPPUNIT_TEST(testInvalidCalls);
	CPPUNIT_TEST_SUITE_END();

public:
	void testValidCalls();
	void testInvalidCalls();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestConfiguration);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestConfiguration, TestConfiguration);

#endif
