#include "JsonReader.hpp"

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

vtkSmartPointer<vtkPolyData> JsonReader::getVtkDataSet(int zoomLevel) {
    
    
    

    
    
    vtkSmartPointer<vtkPolyData> dataSet = vtkSmartPointer<vtkPolyData>::New();
    
    vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
    
    
    
    QList<DataPoint> dataPoints = this->pointDataSet.getDataPoints();
    
    
    QList<DataPoint> relevantZoomLevelDataPoints = QList<DataPoint>();
    
    for(QList<DataPoint>::iterator iterator = dataPoints.begin();iterator!=dataPoints.end();iterator++) {
        
        //all prioritys which are less or eqla to the zoomLevel will be written into the vtkPolyData
        if(iterator->getPriority() <= zoomLevel) {
            
            //add point to vtkPoints newPoints
            newPoints->InsertNextPoint(iterator->getCoordinate().lat(), iterator->getCoordinate().lon(), 0);
            
            relevantZoomLevelDataPoints.append(*iterator);
            
            //newScalars->InsertNextValue(iterator->get)
        }
        
    }
    
    dataSet->SetPoints(newPoints);
    
    switch (this->dataType) {
            
        case DataType::CITIES: {
            //Cities
            vtkSmartPointer<vtkCharArray> cityNames = vtkSmartPointer<vtkCharArray>::New();
            
            cityNames->SetNumberOfComponents(1);
            cityNames->SetName("cityNames");

            dataSet->GetPointData()->SetScalars(cityNames);
        }
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

    
    
    
    
    
    vtkCharArray* nameScalars = (vtkCharArray*)dataSet->GetPointData()->GetScalars("cityName");
    
    //nameScalars->PrintSelf(std::cout, vtkIndent(1));
    
    std::cout << "Name des Arrays:" << nameScalars->GetName() << std::endl;
    
    //hier vielleicht den Datentyp übergeben
    //dataSet->SetActiveAttribute(<#vtkInformation *info#>, <#int fieldAssociation#>, <#const char *attributeName#>, <#int attributeType#>)
    
    return dataSet;
}