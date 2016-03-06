#ifndef KRONOS_POINT_COORDINATE_COMPARATOR_HPP
#define KRONOS_POINT_COORDINATE_COMPARATOR_HPP

#include <Utils/Misc/PointCoordinates.hpp>

struct PointCoordinateComparator {
	bool operator()(PointCoordinates a, PointCoordinates b) const {
		return (a.getX() < b.getX()) ||
		       (a.getX() == b.getX() && a.getY() > b.getY()) ||
		       (a.getX() == b.getX() && a.getY() == b.getY() && a.getZ() < b.getZ());
	}
};
#endif