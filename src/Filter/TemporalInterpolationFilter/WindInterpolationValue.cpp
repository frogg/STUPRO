#include <Filter/TemporalInterpolationFilter/WindInterpolationValue.hpp>

WindInterpolationValue::WindInterpolationValue(int priority, int timestamp, float bearing, float speed) : InterpolationValue(priority, timestamp), bearing(bearing), speed(speed) { }
WindInterpolationValue::~WindInterpolationValue() { }

void WindInterpolationValue::setBearing(float bearing) {
	this->bearing = bearing;
}

float WindInterpolationValue::getBearing() const {
	return this->bearing;
}

void WindInterpolationValue::setSpeed(float speed) {
	this->speed = speed;
}

float WindInterpolationValue::getSpeed() const {
	return this->speed;
}