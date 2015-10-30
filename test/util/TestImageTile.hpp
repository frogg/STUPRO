#ifndef KRONOS_TEST_TESTIMAGETILE_HPP
#define KRONOS_TEST_TESTIMAGETILE_HPP

#include <cppunit/extensions/HelperMacros.h>

#include <ImageTile.hpp>

class TestImageTile : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(TestImageTile);
	CPPUNIT_TEST(testMetaData);
	CPPUNIT_TEST(testLayers);
	CPPUNIT_TEST_SUITE_END();

public:
	void testMetaData();
	void testLayers();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestImageTile);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestImageTile, TestImageTile);

#endif
