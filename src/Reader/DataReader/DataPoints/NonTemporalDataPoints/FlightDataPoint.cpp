#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/FlightDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

FlightDataPoint::FlightDataPoint(Coordinate coordinate, int priority, Coordinate destination, QString airlineName, QString originAirportCode, QString destinationAirportCode)
	: NonTemporalDataPoint(Data::FLIGHTS, coordinate, priority), destination(destination), airlineName(airlineName), originAirportCode(originAirportCode), destinationAirportCode(destinationAirportCode) { }

Coordinate FlightDataPoint::getDestination() const {
	return this->destination;
}

QString FlightDataPoint::getAirlineName() const {
    return this->airlineName;
}

QString FlightDataPoint::getOriginAirportCode() const {
	return this->originAirportCode;
}

QString FlightDataPoint::getDestinationAirportCode() const {
	return this->destinationAirportCode;
}