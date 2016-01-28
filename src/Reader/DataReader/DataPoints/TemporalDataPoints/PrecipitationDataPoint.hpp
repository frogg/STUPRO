#ifndef KRONOS_PRECIPITATIONDATAPOINT_HPP
#define KRONOS_PRECIPITATIONDATAPOINT_HPP

#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemporalDataPoint.hpp>

/**
 * Holds a data point for the precipitation rate at a specific location
 */
class PrecipitationDataPoint : public TemporalDataPoint {

public:
	/**
	 * Create a new PrecipitationDataPoint.
	 * @param coordinate The point's coordinates
	 * @param priority The point's zoom level priority
	 * @param timestamp The point's timestamp
	 * @param precipitation The point's precipitation data
	 */
	PrecipitationDataPoint(Coordinate coordinate, int priority, int timestamp,
	                       float precipitationRate);

	/**
	* Get the precipitation rate of this data point.
	* @return The precipitation rate of this data point
	*/
	float getPrecipitationRate() const;

private:
	float precipitationRate;

};

#endif
