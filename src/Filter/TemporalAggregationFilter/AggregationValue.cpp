#include <Filter/TemporalAggregationFilter/AggregationValue.hpp>

AggregationValue::AggregationValue() : timeIndex(0) { }
AggregationValue::~AggregationValue() { }

void AggregationValue::setTimeIndex(int timeIndex) {
    this->timeIndex = timeIndex;
}

int AggregationValue::getTimeIndex() const {
    return this->timeIndex;
}