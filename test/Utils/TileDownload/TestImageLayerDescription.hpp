#ifndef TEST_UTILS_TILEDOWNLOAD_TESTIMAGELAYERDESCRIPTION_HPP
#define TEST_UTILS_TILEDOWNLOAD_TESTIMAGELAYERDESCRIPTION_HPP

#include <cppunit/extensions/AutoRegisterSuite.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestNamer.h>
#include <cppunit/TestCaller.h>

class TestImageLayerDescription : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(TestImageLayerDescription);
	CPPUNIT_TEST(testGetTilePositionFromCoordinates);
	CPPUNIT_TEST_SUITE_END();

public:
	void testGetTilePositionFromCoordinates();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestImageLayerDescription);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestImageLayerDescription, "TestImageLayerDescription");

#endif /* end of include guard: TEST_UTILS_TILEDOWNLOAD_TESTIMAGELAYERDESCRIPTION_HPP */
