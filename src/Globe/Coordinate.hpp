#ifndef STUPRO_GLOBE_COORDINATE_HPP
#define STUPRO_GLOBE_COORDINATE_HPP

/**
 * Holds a lat/long coordinate.
 */
class Coordinate: public Vector2d {
public:

	typedef Vector2d VectorType;

	/**
	 * Constructs a zero coordinate.
	 */
	Coordinate() :
		Vector2d() {
	}

	/**
	 * Constructs a coordinate from lat/lon values.
	 */
	Coordinate(VectorType::ValueType lat, VectorType::ValueType lon) :
		Vector2d(lon, lat) {
	}

	/**
	 * @return the latitude of the coordinate.
	 */
	VectorType::ValueType lat() const {
		return y;
	}

	/**
	 * @return the longitude of the coordinate.
	 */
	VectorType::ValueType lon() const {
		return x;
	}

	/**
	 * Changes the latitude of the coordinate.
	 *
	 * @param lat The new latitude of the coordinate
	 */
	void lat(VectorType::ValueType lat) {
		y = lat;
	}

	/**
	 * Changes the longitude of the coordinate.
	 *
	 * @param lat The new longitude of the coordinate
	 */
	void lon(VectorType::ValueType lon) {
		x = lon;
	}
};

#endif
