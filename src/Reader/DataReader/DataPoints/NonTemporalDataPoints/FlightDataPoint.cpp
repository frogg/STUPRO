#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/FlightDataPoint.hpp>
#include <Reader/DataReader/DataType.hpp>

FlightDataPoint::FlightDataPoint(Coordinate coordinate, int priority, Coordinate destination)
        : NonTemporalDataPoint(DataType::FLIGHTS, coordinate, priority),
            destination(destination) { }

Coordinate FlightDataPoint::getDestination() const {
    return this->destination;
}