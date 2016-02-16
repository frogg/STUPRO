#include <Filter/TemporalAggregationFilter/PrecipitationAggregationValue.hpp>

PrecipitationAggregationValue::PrecipitationAggregationValue() : accumulatedPrecipitation(0) { }
PrecipitationAggregationValue::~PrecipitationAggregationValue() { }

void PrecipitationAggregationValue::setAccumulatedPrecipitation(double accumulatedPrecipitation) {
    this->accumulatedPrecipitation = accumulatedPrecipitation;
}

double PrecipitationAggregationValue::getAccumulatedPrecipitation() const {
    return this->accumulatedPrecipitation;
}