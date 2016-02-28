#include <Filter/TemporalInterpolationFilter/TemperatureInterpolationValue.hpp>

TemperatureInterpolationValue::TemperatureInterpolationValue(int priority, int timestamp,
        float temperature) : InterpolationValue(priority, timestamp), temperature(temperature) { }
TemperatureInterpolationValue::~TemperatureInterpolationValue() { }

void TemperatureInterpolationValue::setTemperature(float temperature) {
	this->temperature = temperature;
}

float TemperatureInterpolationValue::getTemperature() const {
	return this->temperature;
}
