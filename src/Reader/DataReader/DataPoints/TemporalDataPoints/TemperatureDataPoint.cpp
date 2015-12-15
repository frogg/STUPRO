#include "TemperatureDataPoint.hpp"

TemperatureDataPoint::TemperatureDataPoint(int dataType, int coordinate, int priority, int timestamp, float temperature) :TemporalDataPoint(dataType, coordinate, priority, timestamp)
{
	this->temperature = temperature;
}

float TemperatureDataPoint::getTemperature()
{
	return this->temperature;
}
