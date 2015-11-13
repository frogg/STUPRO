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
#include <vector>

class Coordinate
{
public:
    static void logCoordinates(std::vector<Coordinate> &coordinates);

    static Coordinate getCoordinatesFromGlobePoint(double point[], double radiusGlobe);
    static Coordinate getCoordinatesFromPlanePoint(double x, double y, double worldWidth, double worldHeight);

    Coordinate(double latitude, double longitude);

    double getLatitude();
    double getLongitude();
    
private:
    double myLatitude;
    double myLongitude;
};
#endif
