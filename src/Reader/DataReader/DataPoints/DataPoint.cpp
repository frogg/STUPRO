#include "DataPoint.hpp"
#include <iostream>

DataPoint::DataPoint(int dataType, Coordinate coordinate, int priority) {
    
    this->mCoordinate = coordinate;
    this->mDataType = dataType;
    this->mPriority = priority;
    
}


int DataPoint::getDataType() const {
    return this->mDataType;
}

int DataPoint::getPriority() const {
    return this->mPriority;
}

Coordinate DataPoint::getCoordinate() const {
    return this->mCoordinate;
}