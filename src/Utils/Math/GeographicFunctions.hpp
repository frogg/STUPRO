#ifndef KRONOS_GEOGRAPHIC_FUNCTIONS_HPP
#define KRONOS_GEOGRAPHIC_FUNCTIONS_HPP

#include <Globe/Coordinate.hpp>
#include <Utils/Math/Functions.hpp>
#include <math.h>

/**
 * Calculate the distance between two coordinates in kilometres using the Haversine formula.
 * @param startPoint The first point
 * @param endPoint The second point
 * @return The distance between the two points in kilometres
 */
double calculateDistance(Coordinate startPoint, Coordinate endPoint) {
    double earthRadius = 6372.8;

    double latitudeDifference = toRadians(endPoint.lat() - startPoint.lat());
    double longitudeDifference = toRadians(endPoint.lon() - endPoint.lat());
    double firstLatitude = toRadians(startPoint.lat());
    double secondLatitude = toRadians(endPoint.lat());

    double a = pow(sin(latitudeDifference / 2.0), 2) + cos(firstLatitude) * cos(secondLatitude) * pow(sin(longitudeDifference / 2.0), 2);
    double c = 2 * asin(sqrt(a));

    return earthRadius * c;
}

#endif