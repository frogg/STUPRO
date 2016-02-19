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
	 * @param airlineName The name of the airline operating this flight
	 * @param originAirportCode The code of the airport that is the origin of this flight
	 * @param destinationAirportCode The code of the airport that is the destination of this flight
	 * @param flightLength The length of this flight in kilometres
	 */
	FlightDataPoint(Coordinate coordinate, int priority, Coordinate destination, QString airlineName, QString originAirportCode, QString destinationAirportCode, double flightLength);

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
	
	/**
	 * Get the code of the airport that is the origin of this flight.
	 * @return The code of the airport that is the origin of this flight
	 */
	QString getOriginAirportCode() const;
	
	/**
	 * Get the code of the airport that is the destination of this flight.
	 * @return The code of the airport that is the destination of this flight
	 */
	QString getDestinationAirportCode() const;
	
	/**
	 * Get the length of this flight.
	 * @return The length of this flight
	 */
	double getFlightLength() const;

private:
	Coordinate destination;
    QString airlineName;
	QString originAirportCode;
	QString destinationAirportCode;
	double flightLength;
};

#endif
