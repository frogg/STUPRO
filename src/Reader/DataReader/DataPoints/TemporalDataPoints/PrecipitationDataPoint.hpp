#ifndef KRONOS_PRECIPITATIONDATAPOINT_HPP
#define KRONOS_PRECIPITATIONDATAPOINT_HPP

#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemporalDataPoint.hpp>

/**
 * Holds a data point for the precipitation rate at a specific location
 */
class PrecipitationDataPoint : public TemporalDataPoint {

public:
	/**
	 * Create a new PrecipitationDataPoint.
	 * @param coordinate The point's coordinates
	 * @param priority The point's zoom level priority
	 * @param timestamp The point's timestamp
	 * @param precipitation The point's precipitation data
	 */
	PrecipitationDataPoint(Coordinate coordinate, int priority, int timestamp,
	                       float precipitationRate, int precipitationType);

	/**
	* Get the precipitation rate of this data point.
	* @return The precipitation rate of this data point
	*/
	float getPrecipitationRate() const;

	/**
	 * Get the precipitation Type of this data point.
	 * The possible Types are: "rain" = 1, "snow" = 2, "sleet" = 3, "hail" = 4, or none applied = 0
	 * @return The precipitation type of this data point
	 */
	int getPrecipitationType() const;

	/**
	* Constant to be used if the data represents a set of tweets.
	*/
	const static int NOPRECIPITATION = 0;
	const static int RAIN = 1;
	const static int SNOW = 2;
	const static int SLEET = 3;
	const static int HAIL = 4;

private:
	float precipitationRate;
	int precipitationType;
};

#endif
