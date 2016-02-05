#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemporalDataPoint.hpp>

<<<<<<< HEAD
TemporalDataPoint::TemporalDataPoint(int dataType, Coordinate coordinate, int priority,
=======
TemporalDataPoint::TemporalDataPoint(Data::Type dataType, Coordinate coordinate, int priority,
>>>>>>> develop
                                     int timestamp) : DataPoint(dataType, coordinate, priority), timestamp(timestamp) { }

int TemporalDataPoint::getTimestamp() const {
	return this->timestamp;
}