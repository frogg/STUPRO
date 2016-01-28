#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

PrecipitationDataPoint::PrecipitationDataPoint(Coordinate coordinate, int priority, int timestamp,
        float precipitationRate, int precipitationType) : TemporalDataPoint(Data::PRECIPITATION,
	                coordinate, priority, timestamp), precipitationRate(precipitationRate),
	precipitationType(precipitationType) { }

float PrecipitationDataPoint::getPrecipitationRate() const {
	return this->precipitationRate;
}

int PrecipitationDataPoint::getPrecipitationType() const {
	return this->precipitationType;
}