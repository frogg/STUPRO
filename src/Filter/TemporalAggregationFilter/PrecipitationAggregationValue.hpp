#ifndef KRONOS_PRECIPITATION_AGGREGATION_VALUE_HPP
#define KRONOS_PRECIPITATION_AGGREGATION_VALUE_HPP

#include <Filter/TemporalAggregationFilter/AggregationValue.hpp>

/**
 * Holds a precipitation data point's aggregated values.
 */
class PrecipitationAggregationValue : AggregationValue {

public:
	/**
	 * Create a new PrecipitationAggregationValue.
	 */
	PrecipitationAggregationValue();
    ~PrecipitationAggregationValue();

	/**
	* Set the accumulated precipitation of the data point.
	* @param accumulatedPrecipitation The new accumulated precipitation
	*/
	void setAccumulatedPrecipitation(double accumulatedPrecipitation);
    
    /**
    * Get the accumulated precipitation of the data point.
    * @return The current accumulated precipitation
    */
    double getAccumulatedPrecipitation() const;

private:
	double accumulatedPrecipitation;
};

#endif
