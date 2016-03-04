#ifndef KRONOS_CLOUD_COVERAGE_INTERPOLATION_VALUE_HPP
#define KRONOS_CLOUD_COVERAGE_INTERPOLATION_VALUE_HPP

#include <Filter/TemporalInterpolationFilter/InterpolationValue.hpp>

/**
 * Holds a cloud coverage data point's value to be interpolated.
 */
class CloudCoverageInterpolationValue : public InterpolationValue {

public:
	/**
	 * Create a new CloudCoverageInterpolationValue.
	 */
	CloudCoverageInterpolationValue(int priority, int timestamp, float cloudCoverage);
	~CloudCoverageInterpolationValue();

	/**
	* Set the cloud coverage of the data point.
	* @param cloudCoverage The new cloud coverage
	*/
	void setCloudCoverage(float cloudCoverage);

	/**
	* Get the cloud coverage of the data point.
	* @return The current cloud coverage
	*/
	float getCloudCoverage() const;

private:
	float cloudCoverage;
};

#endif
