#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/FlightDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

FlightDataPoint::FlightDataPoint(Coordinate coordinate, int priority, Coordinate destination)
        : NonTemporalDataPoint(Data::FLIGHTS, coordinate, priority), destination(destination) { }

Coordinate FlightDataPoint::getDestination() const {
    return this->destination;
}