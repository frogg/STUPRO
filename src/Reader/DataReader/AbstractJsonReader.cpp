//
//  AbstractJsonReader.cpp
//  kronos
//
//  Created by Frederik Riedel on 15.12.15.
//
//

#include "AbstractJsonReader.hpp"

AbstractJsonReader::AbstractJsonReader(rapidjson::Value& jsonDocument, int dataType, bool temporal) : dataType(dataType), temporal(temporal) {
    
    this->jsonDocument = jsonDocument;
    
    
    
}


int AbstractJsonReader::getDataType() const {
    return this->dataType;
}

bool AbstractJsonReader::hasTemporalData() const {
    return temporal;
}

vtkPolyData AbstractJsonReader::getVtkDataSet(int zoomLevel) {
    //hier wird es interessant
}