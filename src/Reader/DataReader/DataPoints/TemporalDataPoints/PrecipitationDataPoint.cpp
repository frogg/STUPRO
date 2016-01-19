#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>
#include <Reader/DataReader/DataType.hpp>

PrecipitationDataPoint::PrecipitationDataPoint(Coordinate coordinate, int priority, int timestamp,
		float precipitationRate, int precipitationType) : TemporalDataPoint(DataType::PRECIPITATION, coordinate, priority,
		timestamp), precipitationRate(precipitationRate), precipitationType(precipitationType) { }

float PrecipitationDataPoint::getPrecipitationRate() const {
	return this->precipitationRate;
}

int PrecipitationDataPoint::getPrecipitationType() const {
	return this->precipitationType;
}

const int PrecipitationDataPoint::RAIN;
const int PrecipitationDataPoint::SNOW;
const int PrecipitationDataPoint::SLEET;
const int PrecipitationDataPoint::HAIL;
const int PrecipitationDataPoint::NOPRECIPITATION;