#include <Utils/Misc/PointCoordinates.hpp>

#include <Utils/Math/Vector3.hpp>
#include <math.h>

double PointCoordinates::getDistanceTo(PointCoordinates other) {
	return sqrt(pow(this->getX() - other.getX(), 2) + pow(this->getY() - other.getY(),
	            2) + pow(this->getZ() - other.getZ(), 2));
}