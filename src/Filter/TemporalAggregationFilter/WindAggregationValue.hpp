#ifndef KRONOS_WIND_AGGREGATION_VALUE_HPP
#define KRONOS_WIND_AGGREGATION_VALUE_HPP

#include <Filter/TemporalAggregationFilter/AggregationValue.hpp>

/**
 * Holds a wind data point's aggregated values.
 */
class WindAggregationValue : AggregationValue {

public:
	/**
	 * Create a new WindAggregationValue.
	 */
	WindAggregationValue();
    ~WindAggregationValue();

	/**
	* Set the average wind bearing of the data point.
	* @param averageBearing The new average wind bearing
	*/
	void setAverageBearing(double averageBearing);
    
    /**
    * Get the average wind bearing of the data point.
    * @return The current average wind bearing
    */
    double getAverageBearing() const;
	
	/**
	* Set the average wind velocity of the data point.
	* @param averageBearing The new average wind velocity
	*/
	void setAverageVelocity(double averageVelocity);
	
	/**
	* Get the average wind velocity of the data point.
	* @return The current average wind velocity
	*/
	double getAverageVelocity() const;

private:
	double averageBearing;
    double averageVelocity;
};

#endif
