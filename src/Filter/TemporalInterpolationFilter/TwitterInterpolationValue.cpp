#include <Filter/TemporalInterpolationFilter/TwitterInterpolationValue.hpp>

TwitterInterpolationValue::TwitterInterpolationValue(int priority, int timestamp,
        float density) : InterpolationValue(priority, timestamp), density(density) { }
TwitterInterpolationValue::~TwitterInterpolationValue() { }

void TwitterInterpolationValue::setDensity(float density) {
	this->density = density;
}

float TwitterInterpolationValue::getDensity() const {
	return this->density;
}
