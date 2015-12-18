#include "PrecipitationDataPoint.hpp"

#include <Reader/DataReader/DataType.hpp>

PrecipitationDataPoint::PrecipitationDataPoint(Coordinate coordinate, int priority, int timestamp, float precipitation) :TemporalDataPoint(DataType::PRECIPITATION, coordinate, priority, timestamp)
{
	this->precipitationRate = precipitationRate;
}

float PrecipitationDataPoint::getPrecipitationRate()
{
	return this->precipitationRate;
}

