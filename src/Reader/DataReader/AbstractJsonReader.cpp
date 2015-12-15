//
//  AbstractJsonReader.cpp
//  kronos
//
//  Created by Frederik Riedel on 15.12.15.
//
//

#include "AbstractJsonReader.hpp"
#include <vtkSmartPointer.h>
#include <vtkDataArray.h>
#include <Reader/DataReader/DataType.hpp>

AbstractJsonReader::AbstractJsonReader(rapidjson::Value& jsonDocument, int dataType, bool temporal) : dataType(dataType), temporal(temporal) {
    
    this->jsonDocument = jsonDocument;
    
    switch (this->dataType) {
            
        case DataType::CITIES:
            //Cities
            break;

        case DataType::FLIGHTS:
            //Flights
            break;
            
        case DataType::TWEETS:
            //Tweets
            break;
            
        case DataType::PRECIPITATION:
            //PRECIPITATION
            break;
            
        case DataType::TEMPERATURE:
            //TEMPERATURE
            break;
            
        case DataType::WIND:
            //WIND
            break;
            
            
        default:
            break;
    }
    
    
}


int AbstractJsonReader::getDataType() const {
    return this->dataType;
}

bool AbstractJsonReader::hasTemporalData() const {
    return temporal;
}

vtkPolyData AbstractJsonReader::getVtkDataSet(int zoomLevel) {
    
    this->pointDataSet.getDataPoints();
    
    /*
    vtkPolyData* polydata = vtkPolyData::New();

    vtkPoints* points = vtkPoints::New();
    
    vtkDataArray* dataArray = vtkDataArray::New();
    
    points->SetData(dataArray);
    
    polydata->SetPoints(points);
    
    return polydata;*/
}