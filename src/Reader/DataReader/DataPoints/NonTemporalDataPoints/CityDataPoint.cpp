#include "CityDataPoint.hpp"


CityDataPoint::CityDataPoint(int dataType, Coordinate coordinate, int priority, QString name):NonTemporalDataPoint(dataType,coordinate,priority) {
    
    this->mName = name;
    
}

QString CityDataPoint::getName() const {
    return this->mName;
}