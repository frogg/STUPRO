#include "TestGlobe.hpp"

#include <cppunit/TestAssert.h>
#include <Globe/Globe.hpp>


void TestGlobe::testResolution() {
    
    
    Globe globe = *new Globe();
    globe.setResolution(Vector2u(42, 42));
    
    
    Vector2u vector = globe.getResolution();
    
    
    CPPUNIT_ASSERT_EQUAL(42,(int)vector.x);
    CPPUNIT_ASSERT_EQUAL(42,(int)vector.y);
    
}


void TestGlobe::testZoomLevel() {

    Globe globe = *new Globe();
    
    globe.setZoomLevel(42);

    CPPUNIT_ASSERT_EQUAL(42,(int)globe.getZoomLevel());
}


void TestGlobe::testPlaneCoordinates() {
    Globe globe = *new Globe();
    
    std::vector<Vector3d> point;
    
    point.push_back(0);
    point.push_back(0);
    point.push_back(0);
    
    std::vector<Coordinate> coordinate = globe.getPlaneCoordinates(point);
    
    //top left corner = (-180 | 0)
    CPPUNIT_ASSERT_EQUAL(-180, coordinate.pop_back());
    CPPUNIT_ASSERT_EQUAL(0, coordinate.pop_back());
    
}