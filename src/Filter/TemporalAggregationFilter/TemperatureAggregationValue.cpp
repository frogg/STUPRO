#include <Filter/TemporalAggregationFilter/TemperatureAggregationValue.hpp>

TemperatureAggregationValue::TemperatureAggregationValue() : averageTemperature(0) { }
TemperatureAggregationValue::~TemperatureAggregationValue() { }

void TemperatureAggregationValue::setAverageTemperature(double averageTemperature) {
	this->averageTemperature = averageTemperature;
}

double TemperatureAggregationValue::getAverageTemperature() const {
	return this->averageTemperature;
}