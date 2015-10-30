#ifndef KRONOS_TEST_TESTJSONUTIL_HPP
#define KRONOS_TEST_TESTJSONUTIL_HPP

#include <cppunit/extensions/HelperMacros.h>

#include <JsonUtil.hpp>

class TestJsonUtil : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(TestJsonUtil);
	CPPUNIT_TEST(testLoadJson);
	CPPUNIT_TEST_SUITE_END();

public:
	void testLoadJson();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestJsonUtil);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestJsonUtil, TestJsonUtil);

#endif
