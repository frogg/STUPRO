#include <Utils/Math/GeographicFunctions.hpp>

double calculateDistance(Coordinate startPoint, Coordinate endPoint) {
	double earthRadius = 6372.8;

	double latitudeDifference = toRadians(endPoint.lat() - startPoint.lat());
	double longitudeDifference = toRadians(endPoint.lon() - startPoint.lat());
	double firstLatitude = toRadians(startPoint.lat());
	double secondLatitude = toRadians(endPoint.lat());

	double a = pow(sin(latitudeDifference / 2.0),
	               2) + cos(firstLatitude) * cos(secondLatitude) * pow(sin(longitudeDifference / 2.0), 2);
	double c = 2 * asin(sqrt(a));

	return earthRadius * c;
}