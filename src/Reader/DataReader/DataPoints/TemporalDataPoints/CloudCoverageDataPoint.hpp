#ifndef KRONOS_CLOUDCOVERAGEDATAPOINT_HPP
#define KRONOS_CLOUDCOVERAGEDATAPOINT_HPP

#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemporalDataPoint.hpp>

/**
* Holds a data point for the CloudCover at a specific location
*/
class CloudCoverageDataPoint : public TemporalDataPoint {

public:
	/**
	* Create a new CloudCoverageDataPoint.
	* @param coordinate The point's coordinates
	* @param priority The point's zoom level priority
	* @param timestamp The point's timestamp
	* @param cloudCoverage The cloud coverage in this point
	*/
	CloudCoverageDataPoint(Coordinate coordinate, int priority, int timestamp, float cloudCoverage);
	/**
	* Get the cloud coverage in this data point. A numerical value between 0 and 1 (inclusive) representing 
	* the percentage of sky occluded by clouds. A value of 0 corresponds to clear sky, 0.4 to scattered clouds, 
	* 0.75 to broken cloud cover, and 1 to completely overcast skies.
	* @return This point's cloud coverage between 0 and 1.
	*/
	float getCloudCoverage() const;
private:
	float cloudCoverage;
};


#endif
