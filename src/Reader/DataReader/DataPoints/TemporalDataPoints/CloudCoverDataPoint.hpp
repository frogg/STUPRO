#ifndef KRONOS_CLOUDCOVERDATAPOINT_HPP
#define KRONOS_CLOUDCOVERDATAPOINT_HPP

#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemporalDataPoint.hpp>

/**
* Holds a data point for the CloudCover at a specific location
*/
class CloudCoverDataPoint : public TemporalDataPoint {

public:
	/**
	* Create a new CloudCoverDataPoint.
	* @param coordinate The point's coordinates
	* @param priority The point's zoom level priority
	* @param timestamp The point's timestamp
	* @param cloudCover The cloudCover in this point
	*/
	CloudCoverDataPoint(Coordinate coordinate, int priority, int timestamp, float cloudCover);
	/**
	* Get the cloudCover in this data point. A numerical value between 0 and 1 (inclusive) representing 
	* the percentage of sky occluded by clouds. A value of 0 corresponds to clear sky, 0.4 to scattered clouds, 
	* 0.75 to broken cloud cover, and 1 to completely overcast skies.
	* @return This point's cloudCover between 0 and 1.
	*/
	float getCloudCover() const;
private:
	float cloudCover;
};


#endif /* KRONOS_CLOUDCOVERDATAPOINT_HPP */
