#ifndef KRONOS_FLIGHTDATAPOINT_HPP
#define KRONOS_FLIGHTDATAPOINT_HPP

#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/NonTemporalDataPoint.hpp>

/**
 * Holds a data point for a flight with its departure and destination location
 */
class FlightDataPoint : public NonTemporalDataPoint {

public:
	/**
	 * Create a new FlightDataPoint.
	 * @param coordinate The point's coordinates
	 * @param priority The point's zoom level priority
	 * @param destination The flight's destination coordinates
	 */
	FlightDataPoint(Coordinate coordinate, int priority, Coordinate destination, QString airlineName);

	/**
	 * Get the destination of the flight this data point describes.
	 * @return The destination of this FlightDataPoint
	 */
	Coordinate getDestination() const;
    
    /**
     * Get the name of the airline operating this flight.
     * @return The operating airline of this flight
     */
    QString getAirlineName() const;

private:
	Coordinate destination;
    QString airlineName;
};

#endif
