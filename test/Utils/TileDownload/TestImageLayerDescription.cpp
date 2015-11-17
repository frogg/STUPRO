#include "TestImageLayerDescription.hpp"

#include <Utils/TileDownload/ImageLayerDescription.hpp>
#include <cppunit/TestAssert.h>

void TestImageLayerDescription::testGetTilePositionFromCoordinates() {
    double latitude = 51.501414;
    double longitude = -0.141554;

    int zoomLevel = 0;
    int tileX;
    int tileY;

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    CPPUNIT_ASSERT_EQUAL(0, tileX);
    CPPUNIT_ASSERT_EQUAL(0, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    CPPUNIT_ASSERT_EQUAL(1, tileX);
    CPPUNIT_ASSERT_EQUAL(0, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    CPPUNIT_ASSERT_EQUAL(3, tileX);
    CPPUNIT_ASSERT_EQUAL(0, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    CPPUNIT_ASSERT_EQUAL(7, tileX);
    CPPUNIT_ASSERT_EQUAL(1, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    CPPUNIT_ASSERT_EQUAL(15, tileX);
    CPPUNIT_ASSERT_EQUAL(3, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    CPPUNIT_ASSERT_EQUAL(31, tileX);
    CPPUNIT_ASSERT_EQUAL(6, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    CPPUNIT_ASSERT_EQUAL(63, tileX);
    CPPUNIT_ASSERT_EQUAL(13, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    CPPUNIT_ASSERT_EQUAL(127, tileX);
    CPPUNIT_ASSERT_EQUAL(27, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    CPPUNIT_ASSERT_EQUAL(255, tileX);
    CPPUNIT_ASSERT_EQUAL(54, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    CPPUNIT_ASSERT_EQUAL(511, tileX);
    CPPUNIT_ASSERT_EQUAL(109, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    CPPUNIT_ASSERT_EQUAL(1023, tileX);
    CPPUNIT_ASSERT_EQUAL(219, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    CPPUNIT_ASSERT_EQUAL(2046, tileX);
    CPPUNIT_ASSERT_EQUAL(438, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    CPPUNIT_ASSERT_EQUAL(4092, tileX);
    CPPUNIT_ASSERT_EQUAL(876, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    CPPUNIT_ASSERT_EQUAL(8185, tileX);
    CPPUNIT_ASSERT_EQUAL(1752, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    CPPUNIT_ASSERT_EQUAL(16371, tileX);
    CPPUNIT_ASSERT_EQUAL(3504, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    CPPUNIT_ASSERT_EQUAL(32742, tileX);
    CPPUNIT_ASSERT_EQUAL(7008, tileY);
}
