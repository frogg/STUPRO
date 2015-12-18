#include "CityDataPoint.hpp"

#include <Reader/DataReader/DataType.hpp>

CityDataPoint::CityDataPoint(Coordinate coordinate, int priority, QString name):NonTemporalDataPoint(DataType::CITIES,coordinate,priority) {
    
    this->mName = name;
    
}

QString CityDataPoint::getName() const {
    return this->mName;
}