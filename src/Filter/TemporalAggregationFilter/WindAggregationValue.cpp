#include <Filter/TemporalAggregationFilter/WindAggregationValue.hpp>

WindAggregationValue::WindAggregationValue() : averageBearing(0), averageVelocity(0) { }
WindAggregationValue::~WindAggregationValue() { }

void WindAggregationValue::setAverageBearing(double averageBearing) {
	this->averageBearing = averageBearing;
}

double WindAggregationValue::getAverageBearing() const {
	return this->averageBearing;
}

void WindAggregationValue::setAverageVelocity(double averageVelocity) {
	this->averageVelocity = averageVelocity;
}

double WindAggregationValue::getAverageVelocity() const {
	return this->averageVelocity;
}