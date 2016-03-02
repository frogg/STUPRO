#ifndef KRONOS_TEMPERATURE_INTERPOLATION_VALUE_HPP
#define KRONOS_TEMPERATURE_INTERPOLATION_VALUE_HPP

#include <Filter/TemporalInterpolationFilter/InterpolationValue.hpp>

/**
 * Holds a temperature data point's value to be interpolated.
 */
class TemperatureInterpolationValue : public InterpolationValue {

public:
	/**
	 * Create a new TemperatureInterpolationValue.
	 */
	TemperatureInterpolationValue(int priority, int timestamp, float temperature);
	~TemperatureInterpolationValue();

	/**
	* Set the temperature of the data point.
	* @param temperature The new temperature
	*/
	void setTemperature(float temperature);

	/**
	* Get the temperature of the data point.
	* @return The current temperature
	*/
	float getTemperature() const;

private:
	float temperature;
};

#endif
