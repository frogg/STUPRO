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