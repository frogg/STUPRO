#ifndef KRONOS_TEMPORALDATAPOINT_HPP
#define KRONOS_TEMPORALDATAPOINT_HPP

#include <Reader/DataReader/DataPoints/DataPoint.hpp>
#include <Globe/Coordinate.hpp>

/**
 * Holds a data point for piece of temporal data. Therefore it is time-sensitive and contains a
 * UNIX GMT timestamp. This applies to dynamic data like a list of tweets or weather data.
 */
class TemporalDataPoint : public DataPoint {

public:
	/**
	 * Create a new TemporalDataPoint.
	 * @param dataType The point's data type
	 * @param coordinate The point's coordinates
	 * @param priority The point's zoom level priority
	 */
	TemporalDataPoint(int dataType, Coordinate coordinate, int priority, int timestamp);

	/**
	* Get the timestamp of the data in this point.
	* @return This point's timestamp in UNIX GMT format
	*/
	int getTimestamp() const;

private:
	int timestamp;

};

#endif
