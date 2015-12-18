#include "TemperatureDataPoint.hpp"

#include <Reader/DataReader/DataType.hpp>

TemperatureDataPoint::TemperatureDataPoint(Coordinate coordinate, int priority, int timestamp, float temperature) :TemporalDataPoint(DataType::TEMPERATURE, coordinate, priority, timestamp)
{
	this->temperature = temperature;
}

float TemperatureDataPoint::getTemperature() const
{
	return this->temperature;
}
