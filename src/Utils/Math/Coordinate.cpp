//
//  Coordinate.cpp
//  exe
//
//  Created by Larissa Laich on 05.11.15.
//
//

#include <Utils/Math/Coordinate.hpp>
#include <cmath>
#include <iostream>

void Coordinate::logCoordinates(std::vector<Coordinate> &coordinates){
    for (unsigned i=0; i < coordinates.size(); i++) {
        std::cout << "Coordinates " << i << " long: " << coordinates[i].myLongitude << " lat: " << coordinates[i].myLatitude << std::endl;
    }
}

//return by value
Coordinate Coordinate::getCoordinatesFromGlobePoint(double point[], double radiusGlobe){
    //better to use a smart pointer here?
    double radius = radiusGlobe;
    float PI = 3.14159268;
    //coordinate system are not the same as in math formulas
    float x = point[0];
    float y = point[2];
    float z = point[1];

    //created at stack -> don't have to delete it manually
    Coordinate coordinate (asin(z/radius) *180/PI, atan2(x, y)  * 180/PI);
    //coordinate.latitude = asin(z/radius) *180/PI;
    //coordinate.longitude = atan2(x, y)  * 180/PI ;
    return coordinate;
}

Coordinate Coordinate::getCoordinatesFromPlanePoint(double x, double y, double worldWidth, double worldHeight) {
    double latitude = x / (worldWidth / 2) * 180;
    double longitude = y / (worldHeight / 2) * 90;

    return Coordinate(latitude, longitude);
}

double Coordinate::getLatitude(){
    return myLatitude;
}

double Coordinate::getLongitude(){
    return myLongitude;
}

Coordinate::Coordinate(double latitude, double longitude) : myLatitude(latitude), myLongitude(longitude){
}
