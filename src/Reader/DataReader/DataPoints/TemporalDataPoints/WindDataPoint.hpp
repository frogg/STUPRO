#ifndef KRONOS_WINDDATAPOINT_HPP
#define KRONOS_WINDDATAPOINT_HPP

#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemporalDataPoint.hpp>

/**
 * Holds a data point for the wind speed and direction at a specific location
 */
class WindDataPoint : public TemporalDataPoint {

public:
	/**
	 * Create a new WindDataPoint.
	 * @param coordinate The point's coordinates
	 * @param priority The point's zoom level priority
	 * @param timestamp The point's timestamp
	 * @param direction The wind's direction in this point
	 * @param speed The wind's speed in this point
	 */
	WindDataPoint(Coordinate coordinate, int priority, int timestamp, float direction, float speed);

	/**
	 * Get the wind direction in this data point.
	 * @return This point's wind direction in degrees.
	 */
	float getDirection() const;

	/**
	 * Get the wind speed in this data point.
	 * @return This point's wind speed in metres per second.
	 */
	float getSpeed() const;

private:
	float direction;
	float speed;

};

#endif
