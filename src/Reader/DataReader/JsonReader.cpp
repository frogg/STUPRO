#include <Reader/DataReader/JsonReader.hpp>

#include <Reader/DataReader/DataType.hpp>
#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/CityDataPoint.hpp>
#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/FlightDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemperatureDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/TweetDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/WindDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/CloudCoverDataPoint.hpp>

#include <Reader/DataReader/PolyDataSetHelper.hpp>
#include <Utils/Config/Configuration.hpp>

JsonReader::JsonReader(rapidjson::Value& jsonDocument, int dataType, bool temporal,
    int timeResolution) : dataType(dataType), temporal(temporal), timeResolution(timeResolution) {
    this->cachingEnabled = true;
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
			case DataType::PRECIPITATION: {
				int precipitationType = PrecipitationDataPoint::NOPRECIPITATION;
                
				if (QString(jsonValue[i]["precipitationType"].GetString()) == "rain") {
					precipitationType = PrecipitationDataPoint::RAIN;
				} else if (QString(jsonValue[i]["precipitationType"].GetString()) == "snow") {
					precipitationType = PrecipitationDataPoint::SNOW;
				} else if (QString(jsonValue[i]["precipitationType"].GetString()) == "sleet") {
					precipitationType = PrecipitationDataPoint::SLEET;
				} else if (QString(jsonValue[i]["precipitationType"].GetString()) == "hail") {
					precipitationType = PrecipitationDataPoint::HAIL;
				}

				dataPoint = new PrecipitationDataPoint(
					Coordinate(
    					jsonValue[i]["latitude"].GetDouble(),
    					jsonValue[i]["longitude"].GetDouble()
					),
					depth,
					jsonValue[i]["timestamp"].GetInt(),
					(float) jsonValue[i]["precipitationRate"].GetDouble(),
					precipitationType
				);
				break; }
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

void JsonReader::setCachingEnabled(bool cachingEnabled) {
    this->cachingEnabled = cachingEnabled;
}

bool JsonReader::isCachingEnabled() const {
    return this->cachingEnabled;
}

void JsonReader::cacheAllData() {
    this->setCachingEnabled(true);
    
    for (int i = 0; i <= Configuration::getInstance().getInteger("dataReader.maximumPriority");
            i++) {
        this->getVtkDataSet(i);
    }
}

void JsonReader::clearCache() {
    this->cache.clear();
}

vtkSmartPointer<vtkPolyData> JsonReader::getVtkDataSet(int zoomLevel) {
    // If possible, retrieve the data set from the cache
    if (this->cachingEnabled && this->cache.contains(zoomLevel)) {
        return this->cache.value(zoomLevel);
    }
    
    vtkSmartPointer<vtkPolyData> dataSet;
    
    if (this->hasTemporalData()) {
        // TODO: Enable temporal data points
        dataSet = PolyDataSetHelper::getPolyDataFromDataPoints(
            this->pointDataSet,
            zoomLevel,
            this->dataType,
            0,
            0,
            0
        );
    } else {
        dataSet = PolyDataSetHelper::getPolyDataFromDataPoints(
            this->pointDataSet,
            zoomLevel,
            this->dataType
        );
    }
    
    // Save the data set to the cached if desired
    if (this->cachingEnabled) {
        this->cache.insert(zoomLevel, dataSet);
    }
    
    return dataSet;
}