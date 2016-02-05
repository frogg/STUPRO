#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemperatureDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

TemperatureDataPoint::TemperatureDataPoint(Coordinate coordinate, int priority, int timestamp,
<<<<<<< HEAD
        float temperature) : TemporalDataPoint(DataType::TEMPERATURE, coordinate, priority,
=======
        float temperature) : TemporalDataPoint(Data::TEMPERATURE, coordinate, priority,
>>>>>>> develop
	                timestamp), temperature(temperature) { }

float TemperatureDataPoint::getTemperature() const {
	return this->temperature;
}
