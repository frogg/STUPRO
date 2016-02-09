#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemperatureDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

TemperatureDataPoint::TemperatureDataPoint(Coordinate coordinate, int priority, int timestamp,

        float temperature) : TemporalDataPoint(Data::TEMPERATURE, coordinate, priority,

	                timestamp), temperature(temperature) { }

float TemperatureDataPoint::getTemperature() const {
	return this->temperature;
}
