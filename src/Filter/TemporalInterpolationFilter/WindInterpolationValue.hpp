#ifndef KRONOS_WIND_INTERPOLATION_VALUE_HPP
#define KRONOS_WIND_INTERPOLATION_VALUE_HPP

#include <Filter/TemporalInterpolationFilter/InterpolationValue.hpp>

/**
 * Holds a wind data point's value to be interpolated.
 */
class WindInterpolationValue : public InterpolationValue {

public:
	/**
	 * Create a new WindInterpolationValue.
	 */
	WindInterpolationValue(int priority, int timestamp, float bearing, float speed);
	~WindInterpolationValue();

	/**
	 * Set the wind bearing of the data point.
	 * @param bearing The new wind bearing
	 */
	void setBearing(float bearing);

	/**
	 * Get the wind bearing of the data point.
	 * @return The current wind bearing
	 */
	float getBearing() const;
	
	/**
	 * Set the wind speed of the data point.
	 * @param speed The new wind speed
	 */
	void setSpeed(float speed);
	
	/**
	 * Get the wind speed of this data point.
	 * @return The wind speed of this data point
	 */
	float getSpeed() const;

private:
	float bearing;
	float speed;
};

#endif
