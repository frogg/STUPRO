#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/FlightDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

FlightDataPoint::FlightDataPoint(Coordinate coordinate, int priority, Coordinate destination, QString airlineName)
	: NonTemporalDataPoint(Data::FLIGHTS, coordinate, priority), destination(destination), airlineName(airlineName) { }

Coordinate FlightDataPoint::getDestination() const {
	return this->destination;
}

QString FlightDataPoint::getAirlineName() const {
    return this->airlineName;
}