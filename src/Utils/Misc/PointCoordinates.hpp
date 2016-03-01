#ifndef KRONOS_POINT_COORDINATES_HPP
#define KRONOS_POINT_COORDINATES_HPP

#include <Utils/Math/Vector3.hpp>

/**
 * Holds three-dimensional coordinates of a point.
 */
class PointCoordinates : public Vector3d {
public:
	typedef Vector3d VectorType;

	/**
	 * Create new empty point coordinates.
	 */
	PointCoordinates() : Vector3d() { }

	/**
	 * Create new point coordinates.
	 * @param x The x coordinates of the point
	 * @param y The y coordinates of the point
	 * @param z The z coordinates of the point
	 */
	PointCoordinates(VectorType::ValueType x, VectorType::ValueType y,
	                 VectorType::ValueType z) : Vector3d(x, y, z) { }

	VectorType::ValueType getX() const {
		return x;
	}

	VectorType::ValueType getY() const {
		return y;
	}

	VectorType::ValueType getZ() const {
		return z;
	}

	bool operator< (const PointCoordinates& other) const {
		return (this->getX() < other.getX()) ||
		       (this->getX() == other.getX() && this->getY() < other.getY()) ||
		       (this->getX() == other.getX() && this->getY() == other.getY() && this->getZ() < other.getZ());
	}

	bool operator== (const PointCoordinates& other) const {
		return (this->getX() == other.getX() && this->getY() == other.getY()
		        && this->getZ() == other.getZ());
	}

	/**
	 * Get the distance of this point to some other coordinates.
	 * @param other Some other coordinates
	 * @return The distance of these to the other coordinates
	 */
	double getDistanceTo(PointCoordinates other);
};

#endif
