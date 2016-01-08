#include "JsonReader.hpp"

#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkDataArray.h>
#include <vtkStringArray.h>
#include <vtkTypeInt32Array.h>
#include <vtkTypeFloat32Array.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include "vtkCellArray.h"

#include <Reader/DataReader/DataType.hpp>
#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/CityDataPoint.hpp>
#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/FlightDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemperatureDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/TweetDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/WindDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/CloudCoverDataPoint.hpp>

#include <Utils/Config/Configuration.hpp>

JsonReader::JsonReader(rapidjson::Value& jsonDocument, int dataType, bool temporal) : 
        dataType(dataType), temporal(temporal) {
    this->pointDataSet = PointDataSet();
    this->indexDataPoints(jsonDocument["children"], 0);
}

void JsonReader::indexDataPoints(rapidjson::Value& jsonValue, int depth) {
    for (rapidjson::SizeType i = 0; i < jsonValue.Size(); i++) {
        DataPoint* dataPoint;
        
        // Initialize the new data point depending on the data type
        switch (this->dataType) {
            case DataType::CITIES:
                dataPoint = new CityDataPoint(
                    Coordinate(
                        jsonValue[i]["latitude"].GetDouble(),
                        jsonValue[i]["longitude"].GetDouble()
                    ),
                    depth,
                    jsonValue[i]["name"].GetString()
                );
                break;
            case DataType::FLIGHTS:
                dataPoint = new FlightDataPoint(
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
                dataPoint = new TweetDataPoint(
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
                dataPoint = new PrecipitationDataPoint(
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
                dataPoint = new TemperatureDataPoint(
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
                dataPoint = new WindDataPoint(
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
			case DataType::CLOUDCOVER:
				dataPoint = new CloudCoverDataPoint(
					Coordinate(
					    jsonValue[i]["latitude"].GetDouble(),
					    jsonValue[i]["longitude"].GetDouble()
					),
					depth,
					jsonValue[i]["timestamp"].GetInt(),
					(float) jsonValue[i]["cloudCover"].GetDouble()
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
    // Create an empty data set and a point array the data set will contain
    vtkSmartPointer<vtkPolyData> dataSet = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    
    // Create an empty vert data set
    vtkCellArray *verts = vtkCellArray::New();
    
    // Iterate over all data points and extract those that are relevant with respect to the given
    // zoom level
    QList<DataPoint*> relevantDataPoints = QList<DataPoint*>();
        
    for (int i = 0; i < this->pointDataSet.getDataPoints().size(); i++) {
        DataPoint* point = this->pointDataSet.getDataPoints().at(i);
        if (point->getPriority() <= zoomLevel) {
            relevantDataPoints.append(point);
        }
    }
    
    // Add 
    
    verts->Allocate(verts->EstimateSize(1,relevantDataPoints.size()));

    verts->InsertNextCell(relevantDataPoints.size());
    
    
    // Create some data arrays all temporal data sets have in common. The timestamp array will not
    // be used if the data set is non-temporal.
    
    // An integer array containing the priority of each data point. This is added as a convenience
    // measure for potentially using it later on with `vtkPointSetToLabelHierarchy`.
    vtkSmartPointer<vtkTypeInt32Array> priorities = vtkSmartPointer<vtkTypeInt32Array>::New();
    priorities->SetNumberOfComponents(relevantDataPoints.size());
    priorities->SetName("priorities");
    
    // An integer array containing the timestamp of each data point. 32-bit integers are sufficient
    // since UNIX timestamps are being used.
    vtkSmartPointer<vtkTypeInt32Array> timestamps = vtkSmartPointer<vtkTypeInt32Array>::New();
    timestamps->SetNumberOfComponents(relevantDataPoints.size());
    timestamps->SetName("timestamps");
    
    // Iterate through all relevant data points, add their coordinates as new points and fill the
    // array of priority values.
    for (QList<DataPoint*>::iterator iterator = relevantDataPoints.begin();
            iterator != relevantDataPoints.end(); ++iterator) {
        // Insert the new point with its longitude, latitude and a height of zero
        // and add point to verts
        verts->InsertCellPoint(points->InsertNextPoint(
            (*iterator)->getCoordinate().lon(),
            (*iterator)->getCoordinate().lat(),
            0
        ));
        
        
        
        // Invert the priority before adding it since with `vtkPointSetToLabelHierarchy`,
        // higher priority values are more visible, which is the other way around than
        // the definition in these `DataReader` classes.
        priorities->InsertNextValue(
            Configuration::getInstance().getInteger("dataReader.maximumPriority")
            - (*iterator)->getPriority()
        );
        
        // Add timestamps if they exist
        if (this->hasTemporalData()) {
            const TemporalDataPoint* temporalDataPoint = dynamic_cast<const TemporalDataPoint*>(
                (*iterator)
            );
            
            timestamps->InsertNextValue(temporalDataPoint->getTimestamp());
        }
    }

    dataSet->SetPoints(points);
    
    dataSet->SetVerts(verts);
    
    // Add relevant data arrays depending on the data type
    switch (this->dataType) {
        case DataType::CITIES: {
            vtkSmartPointer<vtkStringArray> cityNames = vtkSmartPointer<vtkStringArray>::New();
            cityNames->SetNumberOfComponents(relevantDataPoints.size());
            cityNames->SetName("names");
            
            for(QList<DataPoint*>::iterator iterator = relevantDataPoints.begin();
                    iterator != relevantDataPoints.end(); ++iterator) {
                const CityDataPoint* dataPoint = dynamic_cast<const CityDataPoint*>(
            		(*iterator)
            	);
                
                cityNames->InsertNextValue(dataPoint->getName().toStdString());
            }

            dataSet->GetPointData()->AddArray(cityNames);
            break;
        }
            
        case DataType::FLIGHTS: {
            vtkSmartPointer<vtkDoubleArray> destinations = vtkSmartPointer<vtkDoubleArray>::New();
            // Twice the amount of components since destination coordinates are tuples
            destinations->SetNumberOfComponents(relevantDataPoints.size() * 2);
            destinations->SetName("destinations");
            
            for(QList<DataPoint*>::iterator iterator = relevantDataPoints.begin();
                    iterator != relevantDataPoints.end(); ++iterator) {
                const FlightDataPoint* dataPoint = dynamic_cast<const FlightDataPoint*>(
                    (*iterator)
                );
                
                // Insert the new destination as a new tuple of latitude and longitude
                destinations->InsertNextTuple2(
                    dataPoint->getDestination().lat(),
                    dataPoint->getDestination().lon()
                );
            }

            dataSet->GetPointData()->AddArray(destinations);
            break;
        }
            
        case DataType::TWEETS: {
            vtkSmartPointer<vtkStringArray> authors = vtkSmartPointer<vtkStringArray>::New();
            authors->SetNumberOfComponents(relevantDataPoints.size());
            authors->SetName("authors");
            
            vtkSmartPointer<vtkStringArray> contents = vtkSmartPointer<vtkStringArray>::New();
            contents->SetNumberOfComponents(relevantDataPoints.size());
            contents->SetName("contents");
            
            for(QList<DataPoint*>::iterator iterator = relevantDataPoints.begin();
                    iterator != relevantDataPoints.end(); ++iterator) {
                const TweetDataPoint* dataPoint = dynamic_cast<const TweetDataPoint*>(
                    (*iterator)
                );
                
                authors->InsertNextValue(dataPoint->getAuthor().toStdString());
                contents->InsertNextValue(dataPoint->getContent().toStdString());
            }

            dataSet->GetPointData()->AddArray(authors);
            dataSet->GetPointData()->AddArray(contents);
            break;
        }
            
        case DataType::PRECIPITATION: {
            vtkSmartPointer<vtkTypeFloat32Array> precipitationRates
                = vtkSmartPointer<vtkTypeFloat32Array>::New();
            precipitationRates->SetNumberOfComponents(relevantDataPoints.size());
            precipitationRates->SetName("precipitationRates");
            
            for(QList<DataPoint*>::iterator iterator = relevantDataPoints.begin();
                    iterator != relevantDataPoints.end(); ++iterator) {
                const PrecipitationDataPoint* dataPoint
                    = dynamic_cast<const PrecipitationDataPoint*>((*iterator));
                
                precipitationRates->InsertNextValue(dataPoint->getPrecipitationRate());
            }

            dataSet->GetPointData()->AddArray(precipitationRates);
            break;
        }
            
        case DataType::TEMPERATURE: {
            vtkSmartPointer<vtkTypeFloat32Array> temperatures
                = vtkSmartPointer<vtkTypeFloat32Array>::New();
            temperatures->SetNumberOfComponents(relevantDataPoints.size());
            temperatures->SetName("temperatures");
            
            for(QList<DataPoint*>::iterator iterator = relevantDataPoints.begin();
                    iterator != relevantDataPoints.end(); ++iterator) {
                const TemperatureDataPoint* dataPoint
                    = dynamic_cast<const TemperatureDataPoint*>((*iterator));
                
                temperatures->InsertNextValue(dataPoint->getTemperature());
            }

            dataSet->GetPointData()->AddArray(temperatures);
            break;
        }
            
        case DataType::WIND: {
            vtkSmartPointer<vtkTypeFloat32Array> speeds
                = vtkSmartPointer<vtkTypeFloat32Array>::New();
            speeds->SetNumberOfComponents(relevantDataPoints.size());
            speeds->SetName("speeds");
            
            vtkSmartPointer<vtkTypeFloat32Array> directions
                = vtkSmartPointer<vtkTypeFloat32Array>::New();
            directions->SetNumberOfComponents(relevantDataPoints.size());
            directions->SetName("directions");
            
            for(QList<DataPoint*>::iterator iterator = relevantDataPoints.begin();
                    iterator != relevantDataPoints.end(); ++iterator) {
                const WindDataPoint* dataPoint
                    = dynamic_cast<const WindDataPoint*>((*iterator));
                
                speeds->InsertNextValue(dataPoint->getSpeed());
                directions->InsertNextValue(dataPoint->getDirection());
            }

            dataSet->GetPointData()->AddArray(speeds);
            dataSet->GetPointData()->AddArray(directions);
            break;
        }
            
        case DataType::CLOUDCOVER: {
            vtkSmartPointer<vtkTypeFloat32Array> coverageValues
                = vtkSmartPointer<vtkTypeFloat32Array>::New();
            coverageValues->SetNumberOfComponents(relevantDataPoints.size());
            coverageValues->SetName("cloudCovers");
            
            for(QList<DataPoint*>::iterator iterator = relevantDataPoints.begin();
                    iterator != relevantDataPoints.end(); ++iterator) {
                const CloudCoverDataPoint* dataPoint
                    = dynamic_cast<const CloudCoverDataPoint*>((*iterator));
                
                coverageValues->InsertNextValue(dataPoint->getCloudCover());
            }

            dataSet->GetPointData()->AddArray(coverageValues);
            break;
        }
    }
    
    // Finally, add the aforementioned common arrays if necessary
    if (this->hasTemporalData()) {
        dataSet->GetPointData()->AddArray(timestamps);
    }
    
    dataSet->GetPointData()->AddArray(priorities);
    
    return dataSet;
}