#include <gtest/gtest.h>
#include <Globe/Globe.hpp>

TEST(TestGlobe, resolution) {
    /*
    Globe globe = *new Globe();
    globe.setResolution(Vector2u(42, 42));

    Vector2u vector = globe.getResolution();

    CPPUNIT_ASSERT_EQUAL(42,(int)vector.x);
    CPPUNIT_ASSERT_EQUAL(42,(int)vector.y);
    */
}

TEST(TestGlobe, zoomLevel) {
    /*
    Globe globe = *new Globe();

    globe.setZoomLevel(42);

    CPPUNIT_ASSERT_EQUAL(42,(int)globe.getZoomLevel());
    */
}

TEST(TestGlobe, planeCoordinates) {
    /*
    Globe globe = *new Globe();

    std::vector<Vector3d> point;

    point.push_back(0);
    point.push_back(0);
    point.push_back(0);

    std::vector<Coordinate> coordinate = globe.getPlaneCoordinates(point);

    //top left corner = (-180 | 0)
    CPPUNIT_ASSERT_EQUAL(-180, coordinate.pop_back());
    CPPUNIT_ASSERT_EQUAL(0, coordinate.pop_back());
    */
}
