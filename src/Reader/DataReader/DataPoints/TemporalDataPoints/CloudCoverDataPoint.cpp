#include <Reader/DataReader/DataPoints/TemporalDataPoints/CloudCoverDataPoint.hpp>
#include <Reader/DataReader/DataType.hpp>

CloudCoverDataPoint::CloudCoverDataPoint(Coordinate coordinate, int priority, int timestamp, 
	float cloudCover) : TemporalDataPoint(DataType::CLOUDCOVER, coordinate, priority, timestamp),
	cloudCover(cloudCover) { }

float CloudCoverDataPoint::getCloudCover() const {
	return this->cloudCover;
}