#include <Reader/DataReader/DataPoints/DataPoint.hpp>

DataPoint::DataPoint(Data::Type dataType, Coordinate coordinate, int priority)
        : dataType(dataType), coordinate(coordinate), priority(priority) { }

Data::Type DataPoint::getDataType() const {
    return this->dataType;
}

int DataPoint::getPriority() const {
    return this->priority;
}

Coordinate DataPoint::getCoordinate() const {
    return this->coordinate;
}