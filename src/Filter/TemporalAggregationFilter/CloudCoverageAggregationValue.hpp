#ifndef KRONOS_CLOUD_COVERAGE_AGGREGATION_VALUE_HPP
#define KRONOS_CLOUD_COVERAGE_AGGREGATION_VALUE_HPP

#include <Filter/TemporalAggregationFilter/AggregationValue.hpp>

/**
 * Holds a cloud coverage data point's aggregated values.
 */
class CloudCoverageAggregationValue : public AggregationValue {

public:
	/**
	 * Create a new CloudCoverageAggregationValue.
	 */
	CloudCoverageAggregationValue();
	~CloudCoverageAggregationValue();

	/**
	* Set the average cloud coverage of the data point.
	* @param averageCloudCoverage The new average cloud coverage
	*/
	void setAverageCloudCoverage(double averageCloudCoverage);

	/**
	* Get the average cloud coverage of the data point.
	* @return The current average cloud coverage
	*/
	double getAverageCloudCoverage() const;

private:
	double averageCloudCoverage;
};

#endif
