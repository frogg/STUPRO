#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>
#include <Reader/DataReader/DataType.hpp>

PrecipitationDataPoint::PrecipitationDataPoint(Coordinate coordinate, int priority, int timestamp,
		float precipitation) : TemporalDataPoint(DataType::PRECIPITATION, coordinate, priority,
			timestamp), precipitationRate(precipitationRate) { }

float PrecipitationDataPoint::getPrecipitationRate() const {
	return this->precipitationRate;
}

