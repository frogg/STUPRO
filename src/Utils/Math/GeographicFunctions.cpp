#include <Utils/Math/GeographicFunctions.hpp>

double calculateDistance(Coordinate startPoint, Coordinate endPoint) {
    double earthRadius = 6372.8;

    double latitudeDifference = toRadians(endPoint.lat() - startPoint.lat());
    double longitudeDifference = toRadians(endPoint.lon() - endPoint.lat());
    double firstLatitude = toRadians(startPoint.lat());
    double secondLatitude = toRadians(endPoint.lat());

    double a = powf(sin(latitudeDifference / 2.0), 2) + cos(firstLatitude) * cos(secondLatitude) * powf(sin(longitudeDifference / 2.0), 2);
    double c = 2 * asin(sqrt(a));

    return earthRadius * c;
    /*φ lat
    λ lon
    
    double a = sin(Δφ/2) * Math.sin(Δφ/2) +
        Math.cos(φ1) * Math.cos(φ2) *
        Math.sin(Δλ/2) * Math.sin(Δλ/2);
var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));

var d = R * c;*/
}