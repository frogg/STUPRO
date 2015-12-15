#include "FlightDataPoint.hpp"

FlightDataPoint::FlightDataPoint(int dataType, Coordinate coordinate, int priority, Coordinate destination):NonTemporalDataPoint(dataType,coordinate,priority) {
    
    this->mDestination = destination;
    
}

Coordinate FlightDataPoint::getDestination() const {
    return this->mDestination;
}