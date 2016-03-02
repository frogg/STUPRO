#include <Filter/TemporalInterpolationFilter/TwitterInterpolationValue.hpp>

TwitterInterpolationValue::TwitterInterpolationValue(float density) : InterpolationValue(0, 0), density(density) { }
TwitterInterpolationValue::~TwitterInterpolationValue() { }

void TwitterInterpolationValue::setDensity(float density) {
	this->density = density;
}

float TwitterInterpolationValue::getDensity() const {
	return this->density;
}
