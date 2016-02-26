#ifndef KRONOS_INTERPOLATION_VALUE_HPP
#define KRONOS_INTERPOLATION_VALUE_HPP

/**
 * Abstract class that holds a value to be used while interpolating data.
 */
class InterpolationValue {

public:
	/**
	 * Create a new InterpolationValue.
     * @param priority The display priority of this interpolation data point
     * @param timestamp The timestamp of this interpolation data point
	 */
	InterpolationValue(int priority, int timestamp);
	~InterpolationValue();
    
    /**
     * Get the priority of this interpolation data point.
     * @return The current priority
     */
    int getPriority() const;
    
    /**
     * Get the timestamp of this interpolation data point.
     * @return The current timestamp
     */
    int getTimestamp() const;
    
private:
    /**
     * The display priority of this interpolation data point.
     */
    int priority;
    
    /**
     * The timestamp of this interpolation data point.
     */
    int timestamp;
};

#endif
