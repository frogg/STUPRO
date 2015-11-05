//
//  Coordinate.cpp
//  exe
//
//  Created by Larissa Laich on 05.11.15.
//
//

#include "Coordinate.hpp"
//log the 5 Coordinates ()
#include <iostream>

void Coordinate::logCoordinates(Coordinate coordinate[5]){
    for(int i=0; i<5; i++){
        std::cout << "Coordinates " << i << " long: " << coordinate[i].myLongitude << " lat: " << coordinate[i].myLatitude << std::endl;
    }
}

Coordinate* Coordinate::getCoordinatesFromGlobePoint(double point[], double radiusGlobe){
    //better to use a smart pointer here?
    double radius = radiusGlobe;
    float PI = 3.14159268;
    //coordinate system are not the same as in math formulas
    float x = point[0];
    float y = point[2];
    float z = point[1];
    
    Coordinate* coordinate = new Coordinate(asin(z/radius) *180/PI, atan2(x, y)  * 180/PI);
    //coordinate.latitude = asin(z/radius) *180/PI;
    //coordinate.longitude = atan2(x, y)  * 180/PI ;
    return coordinate;
}
double Coordinate::getLatitude(){
    return myLatitude;
}
double Coordinate::getLongitude(){
    return myLongitude;
}

Coordinate::Coordinate(double latitude, double longitude){
    myLongitude = latitude;
    myLongitude = longitude;
}

Coordinate::~Coordinate(){
    //What to do here?
}
