#ifndef KRONOS_NONTEMPORALDATAPOINT_HPP
#define KRONOS_NONTEMPORALDATAPOINT_HPP

#include <Reader/DataReader/DataPoints/DataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

/**
 * Holds a data point for piece of non-temporal data. Therefore it is not time-sensitive and does
 * not contain a timestamp. This applies to static data like a list of cities or possible flight
 * routes.
 */
class NonTemporalDataPoint : public DataPoint {

public:
	/**
	 * Create a new NonTemporalDataPoint.
	 * @param dataType The point's data type
	 * @param coordinate The point's coordinates
	 * @param priority The point's zoom level priority
	 */
	NonTemporalDataPoint(Data::Type dataType, Coordinate coordinate, int priority);

};

#endif
