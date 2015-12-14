#include "DataPoint.hpp"
#include <iostream>

DataPoint::DataPoint(int dataType, Coordinate coordinate, int priority) {
    
    this->mCoordinate = coordinate;
    this->mDataType = dataType;
    this->mPriority = priority;
    
}


int DataPoint::getDataType() {
    return this->mDataType;
}

int DataPoint::getPriority() {
    return this->mPriority;
}

Coordinate DataPoint::getCoordinate() {
    return this->mCoordinate;
}