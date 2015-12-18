#ifndef WindDataPoint_H
#define WindDataPoint_H

#include "TemporalDataPoint.hpp"
/**
* Holds a DataPoint for the Winddata at a specific Coordinate, which further includes its DataType, Coordinate, Priority, winddirection and windspeed
*/
class WindDataPoint : public TemporalDataPoint
{
public:
	WindDataPoint(Coordinate coordinate, int priority, int timestamp, float direction, float speed);
	/**
	* @return The winddirection in degrees from 0� to 360�
	*/
	float getDirection() const;
	/**
	* @return The windspedd in miles per hour
	*/
	float getSpeed() const;
private:
	float direction;
	float speed;
};

#endif /* WindDataPoint_H */
