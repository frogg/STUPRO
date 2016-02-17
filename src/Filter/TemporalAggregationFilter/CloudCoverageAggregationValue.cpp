#include <Filter/TemporalAggregationFilter/CloudCoverageAggregationValue.hpp>

CloudCoverageAggregationValue::CloudCoverageAggregationValue() : averageCloudCoverage(0) { }
CloudCoverageAggregationValue::~CloudCoverageAggregationValue() { }

void CloudCoverageAggregationValue::setAverageCloudCoverage(double averageCloudCoverage) {
	this->averageCloudCoverage = averageCloudCoverage;
}

double CloudCoverageAggregationValue::getAverageCloudCoverage() const {
	return this->averageCloudCoverage;
}