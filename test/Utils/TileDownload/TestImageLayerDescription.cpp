#include <Utils/TileDownload/ImageLayerDescription.hpp>
#include <gtest/gtest.h>

TEST(TestImageLayerDescription, getTilePorisionFromCoordinates) {
    double latitude = 51.501414;
    double longitude = -0.141554;

    int zoomLevel = 0;
    int tileX;
    int tileY;

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    EXPECT_EQ(0, tileX);
    EXPECT_EQ(0, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    EXPECT_EQ(1, tileX);
    EXPECT_EQ(0, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    EXPECT_EQ(3, tileX);
    EXPECT_EQ(0, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    EXPECT_EQ(7, tileX);
    EXPECT_EQ(1, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    EXPECT_EQ(15, tileX);
    EXPECT_EQ(3, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    EXPECT_EQ(31, tileX);
    EXPECT_EQ(6, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    EXPECT_EQ(63, tileX);
    EXPECT_EQ(13, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    EXPECT_EQ(127, tileX);
    EXPECT_EQ(27, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    EXPECT_EQ(255, tileX);
    EXPECT_EQ(54, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    EXPECT_EQ(511, tileX);
    EXPECT_EQ(109, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    EXPECT_EQ(1023, tileX);
    EXPECT_EQ(219, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    EXPECT_EQ(2046, tileX);
    EXPECT_EQ(438, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    EXPECT_EQ(4092, tileX);
    EXPECT_EQ(876, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    EXPECT_EQ(8185, tileX);
    EXPECT_EQ(1752, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    EXPECT_EQ(16371, tileX);
    EXPECT_EQ(3504, tileY);

    ImageLayerDescription::getTilePositionFromCoordinates(latitude, longitude, zoomLevel++, tileX, tileY);
    EXPECT_EQ(32742, tileX);
    EXPECT_EQ(7008, tileY);
}
