#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemporalDataPoint.hpp>

TemporalDataPoint::TemporalDataPoint(int dataType, Coordinate coordinate, int priority,
                                     int timestamp) : DataPoint(dataType, coordinate, priority), timestamp(timestamp) { }

int TemporalDataPoint::getTimestamp() const {
	return this->timestamp;
}