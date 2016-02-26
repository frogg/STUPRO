#include <Filter/TemporalInterpolationFilter/InterpolationValue.hpp>

InterpolationValue::InterpolationValue(int priority, int timestamp) : priority(priority), timestamp(timestamp) { }
InterpolationValue::~InterpolationValue() { }

int InterpolationValue::getPriority() const {
	return this->priority;
}

int InterpolationValue::getTimestamp() const {
	return this->timestamp;
}
