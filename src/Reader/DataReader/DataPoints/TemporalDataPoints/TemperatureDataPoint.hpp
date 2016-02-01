#ifndef KRONOS_TEMPERATUREDATAPOINT_HPP
#define KRONOS_TEMPERATUREDATAPOINT_HPP

#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemporalDataPoint.hpp>

/**
 * Holds a data point for the temperature at a specific location
 */
class TemperatureDataPoint : public TemporalDataPoint {

public:
	/**
	 * Create a new TemperatureDataPoint.
	 * @param coordinate The point's coordinates
	 * @param priority The point's zoom level priority
	 * @param timestamp The point's timestamp
	 * @param temperature The point's temperature data
	 */
	TemperatureDataPoint(Coordinate coordinate, int priority, int timestamp, float temperature);

	/**
	 * Get the temperature of this data point.
	 * @return The temperature of this data point
	 */
	float getTemperature() const;

private:
	float temperature;

};

#endif
