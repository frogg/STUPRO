#ifndef PrecipitationDataPoint_H
#define PrecipitationDataPoint_H

#include "TemporalDataPoint.hpp"
/**
* Holds a DataPoint for the PrecipitationRate at a specific Coordinate, which further includes its DataType, Coordinate and Priority
*/
class PrecipitationDataPoint : public TemporalDataPoint
{
public:
	PrecipitationDataPoint(Coordinate coordinate, int priority, int timestamp, float precipitation);
	/**
	* Get the precipitationrate for a temporal DataPoint
	* @return The precipitation-prediction for the hour after the timestamp
	*/
	float getPrecipitationRate();
private:
	float precipitationRate;
};

#endif /* Precipitation_H */
