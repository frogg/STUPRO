#include <Reader/DataReader/DataPoints/DataPoint.hpp>

const int DataPoint::MAXIMUM_PRIORITY;

DataPoint::DataPoint(int dataType, Coordinate coordinate, int priority)
        : dataType(dataType), coordinate(coordinate), priority(priority) { }

int DataPoint::getDataType() const {
    return this->dataType;
}

int DataPoint::getPriority() const {
    return this->priority;
}

Coordinate DataPoint::getCoordinate() const {
    return this->coordinate;
}