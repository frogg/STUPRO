#include "PrecipitationDataPoint.hpp"

PrecipitationDataPoint::PrecipitationDataPoint(int dataType, Coordinate coordinate, int priority, int timestamp, float precipitation) :TemporalDataPoint(dataType, coordinate, priority, timestamp)
{
	this->precipitationRate = precipitationRate;
}

float PrecipitationDataPoint::getPrecipitationRate()
{
	return this->precipitationRate;
}

