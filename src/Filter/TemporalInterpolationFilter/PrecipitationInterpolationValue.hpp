#ifndef KRONOS_PRECIPITATION_INTERPOLATION_VALUE_HPP
#define KRONOS_PRECIPITATION_INTERPOLATION_VALUE_HPP

#include <Filter/TemporalInterpolationFilter/InterpolationValue.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>

/**
 * Holds a precipitation data point's value to be interpolated.
 */
class PrecipitationInterpolationValue : public InterpolationValue {

public:
	/**
	 * Create a new PrecipitationInterpolationValue.
	 */
	PrecipitationInterpolationValue(int priority, int timestamp, float precipitationRate, PrecipitationDataPoint::PrecipitationType precipitationType);
	~PrecipitationInterpolationValue();

	/**
	 * Set the precipitation rate of the data point.
	 * @param precipitationRate The new precipitation rate
	 */
	void setPrecipitationRate(float precipitationRate);

	/**
	 * Get the precipitation rate of the data point.
	 * @return The current precipitation rate
	 */
	float getPrecipitationRate() const;
	
	/**
	 * Set the precipitation type of the data point.
	 * @param precipitationType The new precipitation type
	 */
	void setPrecipitationType(PrecipitationDataPoint::PrecipitationType precipitationType);
	
	/**
	 * Get the precipitation type of this data point.
	 * @return The precipitation type of this data point
	 */
	PrecipitationDataPoint::PrecipitationType getPrecipitationType() const;

private:
	float precipitationRate;
	PrecipitationDataPoint::PrecipitationType precipitationType;
};

#endif
