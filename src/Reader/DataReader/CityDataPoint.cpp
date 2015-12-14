//
//  CityDataPoint.cpp
//  DataPointThings
//
//  Created by Frederik Riedel on 14.12.15.
//  Copyright © 2015 Quappi. All rights reserved.
//

#include "CityDataPoint.hpp"


CityDataPoint::CityDataPoint(int dataType, Coordinate coordinate, int priority, QString name):NonTemporalDataPoint(dataType,coordinate,priority) {
    
    this->mName = name;
    
}

QString CityDataPoint::getName() {
    return this->mName;
}