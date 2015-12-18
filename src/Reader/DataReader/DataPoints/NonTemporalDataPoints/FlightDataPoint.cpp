#include "FlightDataPoint.hpp"

#include <Reader/DataReader/DataType.hpp>

FlightDataPoint::FlightDataPoint(Coordinate coordinate, int priority, Coordinate destination):NonTemporalDataPoint(DataType::FLIGHTS,coordinate,priority) {
    
    this->mDestination = destination;
    
}

Coordinate FlightDataPoint::getDestination() const {
    return this->mDestination;
}