#ifndef TemporalDataPoint_H
#define TemporalDataPoint_H

#include <Reader/DataReader/DataPoints/DataPoint.hpp>
class Coordinate;

/**
* Holds a temporal DataPoint 
* Data point is time-sensitive and does contain a UNIX GMT timestamp
* temporal DataPoints can be dynamic data e.g. Tweets, Precipitation-, Temperature- or WindData
*/
class TemporalDataPoint :DataPoint
{
public:
	TemporalDataPoint(int dataType, Coordinate coordinate, int priority, int timestamp);
	/**
	* Get the timestamp for a temporal DataPoint
	* @return The timestamp in UNIX GMT format
	*/
	int getTimestamp();
private:
	int timestamp;
};

#endif /* TemporalDataPoint_h */
