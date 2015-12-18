#ifndef TemperatureDataPoint_H
#define TemperatureDataPoint_H

#include "TemporalDataPoint.hpp"
/**
* Holds a DataPoint for the Temperature at a specific Coordinate, which further includes its DataType, Coordinate and Priority 
*/
class TemperatureDataPoint : public TemporalDataPoint
{
public:	
	TemperatureDataPoint(Coordinate coordinate, int priority, int timestamp, float temperature);
	float getTemperature() const;
private:
	/**
	* @return The temperature in degrees Fahrenheit
	*/
	float temperature;
};

#endif /* TemperatureDataPoint_H */
