#include "JsonReader.hpp"

#include <vtkDataArray.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkCharArray.h>
#include <Reader/DataReader/DataType.hpp>

#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/CityDataPoint.hpp>
#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/FlightDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemperatureDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/TweetDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/WindDataPoint.hpp>

JsonReader::JsonReader(rapidjson::Value& jsonDocument, int dataType, bool temporal) : 
        dataType(dataType), temporal(temporal) {
    this->pointDataSet = PointDataSet();
    this->indexDataPoints(jsonDocument["children"], 0);
}

void JsonReader::indexDataPoints(rapidjson::Value& jsonValue, int depth) {
    for (rapidjson::SizeType i = 0; i < jsonValue.Size(); i++) {
        DataPoint dataPoint;
        
        // Initialize the new data point depending on the data type
        switch (this->dataType) {
            case DataType::CITIES:
                dataPoint = CityDataPoint(
                    Coordinate(
                        jsonValue[i]["latitude"].GetDouble(),
                        jsonValue[i]["longitude"].GetDouble()
                    ),
                    depth,
                    jsonValue[i]["name"].GetString()
                );
                break;
            case DataType::FLIGHTS:
                dataPoint = FlightDataPoint(
                    Coordinate(
                        jsonValue[i]["startPosition"]["latitude"].GetDouble(),
                        jsonValue[i]["startPosition"]["longitude"].GetDouble()
                    ),
                    depth,
                    Coordinate(
                        jsonValue[i]["endPosition"]["latitude"].GetDouble(),
                        jsonValue[i]["endPosition"]["longitude"].GetDouble()
                    )
                );
                break;                
            case DataType::TWEETS:
                dataPoint = TweetDataPoint(
                    Coordinate(
                        jsonValue[i]["latitude"].GetDouble(),
                        jsonValue[i]["longitude"].GetDouble()
                    ),
                    depth,
                    jsonValue[i]["timestamp"].GetInt(),
                    jsonValue[i]["author"].GetString(),
                    jsonValue[i]["content"].GetString()
                );
                break;                
            case DataType::PRECIPITATION:
                dataPoint = PrecipitationDataPoint(
                    Coordinate(
                        jsonValue[i]["latitude"].GetDouble(),
                        jsonValue[i]["longitude"].GetDouble()
                    ),
                    depth,
                    jsonValue[i]["timestamp"].GetInt(),
                    (float) jsonValue[i]["precipitationRate"].GetDouble()
                );
                break;                
            case DataType::TEMPERATURE:
                dataPoint = TemperatureDataPoint(
                    Coordinate(
                        jsonValue[i]["latitude"].GetDouble(),
                        jsonValue[i]["longitude"].GetDouble()
                    ),
                    depth,
                    jsonValue[i]["timestamp"].GetInt(),
                    (float) jsonValue[i]["temperature"].GetDouble()
                );
                break;                
            case DataType::WIND:
                dataPoint = WindDataPoint(
                    Coordinate(
                        jsonValue[i]["latitude"].GetDouble(),
                        jsonValue[i]["longitude"].GetDouble()
                    ),
                    depth,
                    jsonValue[i]["timestamp"].GetInt(),
                    (float) jsonValue[i]["direction"].GetDouble(),
                    (float) jsonValue[i]["speed"].GetDouble()
                );
                break;
        }
        
        this->pointDataSet.addPoint(dataPoint);
        
        // Recursively iterate through the children data points
        this->indexDataPoints(jsonValue[i]["children"], depth + 1);
	}
}

int JsonReader::getDataType() const {
    return this->dataType;
}

bool JsonReader::hasTemporalData() const {
    return temporal;
}

vtkSmartPointer<vtkPolyData> JsonReader::getVtkDataSet(int zoomLevel) {
    /*vtkSmartPointer<vtkPolyData> dataSet = vtkSmartPointer<vtkPolyData>::New();
    
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
    
    return dataSet;*/
}