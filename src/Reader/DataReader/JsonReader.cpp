#include "JsonReader.hpp"
#include <vtkSmartPointer.h>
#include <vtkDataArray.h>
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

vtkPolyData JsonReader::getVtkDataSet(int zoomLevel) {
    
    this->pointDataSet.getDataPoints();
    
    /*
    vtkPolyData* polydata = vtkPolyData::New();

    vtkPoints* points = vtkPoints::New();
    
    vtkDataArray* dataArray = vtkDataArray::New();
    
    points->SetData(dataArray);
    
    polydata->SetPoints(points);
    
    return polydata;*/
}