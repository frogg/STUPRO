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
double calculateDistance(Coordinate startPoint, Coordinate endPoint);

#endif