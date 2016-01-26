#include <Reader/DataReader/DataPoints/TemporalDataPoints/WindDataPoint.hpp>
#include <Reader/DataReader/DataType.hpp>

WindDataPoint::WindDataPoint(Coordinate coordinate, int priority, int timestamp, float direction,
		float speed) : TemporalDataPoint(DataType::WIND, coordinate, priority, timestamp),
			direction(direction), speed(speed) { }

float WindDataPoint::getDirection() const {
	return this->direction;
}

float WindDataPoint::getSpeed() const {
	return this->speed;
}

