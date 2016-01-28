#include <Reader/DataReader/DataPoints/TemporalDataPoints/CloudCoverageDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

CloudCoverageDataPoint::CloudCoverageDataPoint(Coordinate coordinate, int priority, int timestamp,
        float cloudCoverage) : TemporalDataPoint(Data::CLOUD_COVERAGE, coordinate, priority, timestamp),
	cloudCoverage(cloudCoverage) { }

float CloudCoverageDataPoint::getCloudCoverage() const {
	return this->cloudCoverage;
}