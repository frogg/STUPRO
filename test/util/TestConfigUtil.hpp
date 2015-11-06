#ifndef KRONOS_TEST_TESTCONFIGUTIL_HPP
#define KRONOS_TEST_TESTCONFIGUTIL_HPP

#include <cppunit/extensions/HelperMacros.h>

#include <ConfigUtil.hpp>

class TestConfigUtil : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(TestConfigUtil);
	CPPUNIT_TEST(testLoadConfig);
	CPPUNIT_TEST_SUITE_END();

public:
	void testLoadConfig();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestConfigUtil);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestConfigUtil, TestConfigUtil);

#endif
