//
//  FlightDataPoint.cpp
//  DataPointThings
//
//  Created by Frederik Riedel on 14.12.15.
//  Copyright © 2015 Quappi. All rights reserved.
//

#include "FlightDataPoint.hpp"

FlightDataPoint::FlightDataPoint(int dataType, Coordinate coordinate, int priority, Coordinate destination):NonTemporalDataPoint(dataType,coordinate,priority) {
    
    this->mDestination = destination;
    
}

Coordinate FlightDataPoint::getDestination() {
    return this->mDestination;
}