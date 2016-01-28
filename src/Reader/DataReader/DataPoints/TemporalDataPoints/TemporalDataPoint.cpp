#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemporalDataPoint.hpp>

TemporalDataPoint::TemporalDataPoint(Data::Type dataType, Coordinate coordinate, int priority,
                                     int timestamp) : DataPoint(dataType, coordinate, priority), timestamp(timestamp) { }

int TemporalDataPoint::getTimestamp() const {
	return this->timestamp;
}