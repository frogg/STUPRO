#ifndef KRONOS_AGGREGATION_VALUE_HPP
#define KRONOS_AGGREGATION_VALUE_HPP

/**
 * Abstract class that holds a value to be used while accumulating data.
 */
class AggregationValue {

public:
	/**
	 * Create a new AggregationValue.
	 */
	AggregationValue();
    ~AggregationValue();

	/**
	* Set the time index of this AggregationValue.
	* @param timeIndex The new time index
	*/
	void setTimeIndex(int timeIndex);
    
    /**
    * Get the time index of this AggregationValue.
    * @return The current time index
    */
    int getTimeIndex() const;

private:
	int timeIndex;
};

#endif
