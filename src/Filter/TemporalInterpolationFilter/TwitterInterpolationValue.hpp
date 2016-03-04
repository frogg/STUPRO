#ifndef KRONOS_TWITTER_INTERPOLATION_VALUE_HPP
#define KRONOS_TWITTER_INTERPOLATION_VALUE_HPP

#include <Filter/TemporalInterpolationFilter/InterpolationValue.hpp>

/**
 * Holds a twitter density data point's value to be interpolated.
 */
class TwitterInterpolationValue : public InterpolationValue {

public:
	/**
	 * Create a new TwitterInterpolationValue.
	 */
	TwitterInterpolationValue(float density);
	~TwitterInterpolationValue();

	/**
	* Set the density of the data point.
	* @param density The new density
	*/
	void setDensity(float density);

	/**
	* Get the density of the data point.
	* @return The current density
	*/
	float getDensity() const;

private:
	float density;
};

#endif
