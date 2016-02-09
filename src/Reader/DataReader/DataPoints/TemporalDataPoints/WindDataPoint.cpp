#include <Reader/DataReader/DataPoints/TemporalDataPoints/WindDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

WindDataPoint::WindDataPoint(Coordinate coordinate, int priority, int timestamp, float direction,

                             float speed) : TemporalDataPoint(Data::WIND, coordinate, priority, timestamp),

	direction(direction), speed(speed) { }

float WindDataPoint::getDirection() const {
	return this->direction;
}

float WindDataPoint::getSpeed() const {
	return this->speed;
}

