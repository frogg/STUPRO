//
//  Coordinate.hpp
//  exe
//
//  Created by Larissa Laich on 05.11.15.
//
//

#ifndef Coordinate_hpp
#define Coordinate_hpp
#include <string>
#include "VTKOpenGL.hpp"

class Coordinate
{
public:
    void logCoordinates(Coordinate coordinate[5]);
    static Coordinate* Coordinate::getCoordinatesFromGlobePoint(double point[], double radiusGlobe);
    double getLatitude();
    double getLongitude();
    Coordinate(double latitude, double longitude);
    ~Coordinate();
    
private:
    double myLatitude;
    double myLongitude;
};
#endif /* Coordinate_hpp */
