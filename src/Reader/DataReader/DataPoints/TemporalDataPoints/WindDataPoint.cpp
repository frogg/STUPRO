#include "WindDataPoint.hpp"

WindDataPoint::WindDataPoint(int dataType, Coordinate coordinate, int priority, int timestamp, float direction, float speed) :TemporalDataPoint(dataType, coordinate, priority, timestamp)
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

