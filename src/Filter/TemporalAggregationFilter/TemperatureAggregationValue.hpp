#ifndef KRONOS_TEMPERATURE_AGGREGATION_VALUE_HPP
#define KRONOS_TEMPERATURE_AGGREGATION_VALUE_HPP

#include <Filter/TemporalAggregationFilter/AggregationValue.hpp>

/**
 * Holds a temperature data point's aggregated values.
 */
class TemperatureAggregationValue : public AggregationValue {

public:
	/**
	 * Create a new TemperatureAggregationValue.
	 */
	TemperatureAggregationValue();
    ~TemperatureAggregationValue();

	/**
	* Set the average temperature of the data point.
	* @param averageTemperature The new average temperature
	*/
	void setAverageTemperature(double averageTemperature);
    
    /**
    * Get the average temperature of the data point.
    * @return The current average temperature
    */
    double getAverageTemperature() const;

private:
	double averageTemperature;
};

#endif
