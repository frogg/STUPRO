#include "WindDataPoint.hpp"

#include <Reader/DataReader/DataType.hpp>

WindDataPoint::WindDataPoint(Coordinate coordinate, int priority, int timestamp, float direction, float speed) :TemporalDataPoint(DataType::WIND, coordinate, priority, timestamp)
{
	this->direction = direction;
	this->speed = speed;
}

float WindDataPoint::getDirection()
{
	return this->direction;
}

float WindDataPoint::getSpeed()
{
	return this->speed;
}

