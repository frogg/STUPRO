#include <Reader/DataReader/DataPoints/TemporalDataPoints/CloudCoverDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>


CloudCoverDataPoint::CloudCoverDataPoint(Coordinate coordinate, int priority, int timestamp,
                                         float cloudCover) : TemporalDataPoint(Data::CLOUD_COVERAGE, coordinate, priority, timestamp),
	cloudCover(cloudCover) { }

float CloudCoverDataPoint::getCloudCover() const {
	return this->cloudCover;
}