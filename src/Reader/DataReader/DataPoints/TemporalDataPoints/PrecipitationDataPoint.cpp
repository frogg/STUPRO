#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

PrecipitationDataPoint::PrecipitationDataPoint(Coordinate coordinate, int priority, int timestamp,

        float precipitationRate, PrecipitationDataPoint::PrecipitationType precipitationType) :
	TemporalDataPoint(Data::PRECIPITATION, coordinate, priority, timestamp),
	precipitationRate(precipitationRate), precipitationType(precipitationType) { }


float PrecipitationDataPoint::getPrecipitationRate() const {
	return this->precipitationRate;
}

PrecipitationDataPoint::PrecipitationType PrecipitationDataPoint::getPrecipitationType() const {
	return this->precipitationType;
}