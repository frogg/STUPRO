#include "TemporalDataPoint.hpp"

TemporalDataPoint::TemporalDataPoint(int dataType, Coordinate coordinate, int priority, int timestamp) : DataPoint(dataType, coordinate, priority)
{
	this->timestamp = timestamp;
}


int TemporalDataPoint::getTimestamp()
{
	return this->timestamp;
}