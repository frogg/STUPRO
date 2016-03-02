#include <Filter/TemporalInterpolationFilter/CloudCoverageInterpolationValue.hpp>

CloudCoverageInterpolationValue::CloudCoverageInterpolationValue(int priority, int timestamp,
        float cloudCoverage) : InterpolationValue(priority, timestamp), cloudCoverage(cloudCoverage) { }
CloudCoverageInterpolationValue::~CloudCoverageInterpolationValue() { }

void CloudCoverageInterpolationValue::setCloudCoverage(float cloudCoverage) {
	this->cloudCoverage = cloudCoverage;
}

float CloudCoverageInterpolationValue::getCloudCoverage() const {
	return this->cloudCoverage;
}