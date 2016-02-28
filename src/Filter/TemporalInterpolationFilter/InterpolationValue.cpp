#include <Filter/TemporalInterpolationFilter/InterpolationValue.hpp>

InterpolationValue::InterpolationValue(int priority, int timestamp) : priority(priority),
	timestamp(timestamp) { }
InterpolationValue::InterpolationValue() : priority(0), timestamp(0) { }
InterpolationValue::~InterpolationValue() { }

int InterpolationValue::getPriority() const {
	return this->priority;
}

int InterpolationValue::getTimestamp() const {
	return this->timestamp;
}
