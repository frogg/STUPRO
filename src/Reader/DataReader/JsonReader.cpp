#include "JsonReader.hpp"
#include <vtkSmartPointer.h>
#include <vtkDataArray.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkCharArray.h>
#include <Reader/DataReader/DataType.hpp>

JsonReader::JsonReader(rapidjson::Value& jsonDocument, int dataType, bool temporal) : dataType(dataType), temporal(temporal) {
    
    // this->jsonDocument = jsonDocument;
    
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


int JsonReader::getDataType() const {
    return this->dataType;
}

bool JsonReader::hasTemporalData() const {
    return temporal;
}

vtkPolyData JsonReader::getVtkDataSet(int zoomLevel) {
    
    
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
    

    
    
    vtkSmartPointer<vtkPolyData> dataSet = vtkSmartPointer<vtkPolyData>::New();
    
    vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
    
    vtkSmartPointer<vtkCharArray> newScalars = vtkSmartPointer<vtkCharArray>::New();
    
    newScalars->SetNumberOfComponents(1);
    newScalars->SetName("Name");
    
    
    QList<DataPoint> dataPoints = this->pointDataSet.getDataPoints();
    
    
    QList<DataPoint>::iterator iterator;
    
    for(iterator = dataPoints.begin();iterator!=dataPoints.end();iterator++) {
        
        //all prioritys which are less or eqla to the zoomLevel will be written into the vtkPolyData
        if(iterator->getPriority() <= zoomLevel) {
            
            //add point to vtkPoints newPoints
            newPoints->InsertNextPoint(iterator->getCoordinate().lat(), iterator->getCoordinate().lon(), 0);
            
            //newScalars->InsertNextValue(iterator->get)
        }
        
        
        
    }
    
    
    dataSet->SetPoints(newPoints);
    dataSet->GetPointData()->SetScalars(newScalars);
    dataSet->GetPointData()->GetScalars("Name");
    
    //hier vielleicht den Datentyp übergeben
    dataSet->SetActiveAttributeInfo(<#vtkInformation *info#>, <#int fieldAssociation#>, <#int attributeType#>, <#const char *name#>, <#int arrayType#>, <#int numComponents#>, <#int numTuples#>)
    
    return dataSet;
}