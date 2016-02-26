#include <Filter/TemporalInterpolationFilter/PrecipitationInterpolationValue.hpp>

PrecipitationInterpolationValue::PrecipitationInterpolationValue(int priority, int timestamp, float precipitationRate, PrecipitationDataPoint::PrecipitationType precipitationType) : InterpolationValue(priority, timestamp), precipitationRate(precipitationRate), precipitationType(precipitationType) { }
PrecipitationInterpolationValue::~PrecipitationInterpolationValue() { }

void PrecipitationInterpolationValue::setPrecipitationRate(float precipitationRate) {
	this->precipitationRate = precipitationRate;
}

float PrecipitationInterpolationValue::getPrecipitationRate() const {
	return this->precipitationRate;
}

void PrecipitationInterpolationValue::setPrecipitationType(PrecipitationDataPoint::PrecipitationType precipitationType) {
	this->precipitationType = precipitationType;
}

PrecipitationDataPoint::PrecipitationType PrecipitationInterpolationValue::getPrecipitationType() const {
	return this->precipitationType;
}