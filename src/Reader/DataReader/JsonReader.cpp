#include <Reader/DataReader/JsonReader.hpp>

#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/CityDataPoint.hpp>
#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/FlightDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemperatureDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/TweetDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/WindDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/CloudCoverageDataPoint.hpp>
#include <Reader/DataReader/JsonValidator.hpp>

#include <Reader/DataReader/PolyDataSetHelper.hpp>
#include <Utils/Config/Configuration.hpp>
#include <Utils/Math/GeographicFunctions.hpp>

JsonReader::JsonReader(rapidjson::Value& jsonDocument, Data::Type dataType, QString path,
                       bool temporal,
                       int timeResolution) : filePath(path), temporal(temporal), dataType(dataType),
	timeResolution(timeResolution) {
	this->cachingEnabled = true;
	this->pointDataSet = PointDataSet();

	if (!jsonDocument.IsObject() || !jsonDocument.HasMember("children")) {
		throw JsonReaderParseException(
		    path,
		    "The file is missing the children list that should be directly inside the root tag."
		);
	}

	this->indexDataPoints(jsonDocument["children"], 0);

	// Get timestamps of the earliest and latest data point if the data is time-sensitive
	if (this->hasTemporalData()) {
		// Initialize minimum and maximum values as smallest and biggest possible UNIX timestamps
		// respectively
		int earliestTimestamp = 2147483647;
		int latestTimestamp = 0;

		QList<DataPoint*>::iterator i;
		QList<DataPoint*> points = this->pointDataSet.getDataPoints();
		for (i = points.begin(); i != points.end(); ++i) {
			const TemporalDataPoint* dataPoint
			    = dynamic_cast<const TemporalDataPoint*>((*i));

			int timestamp = dataPoint->getTimestamp();

			if (timestamp < earliestTimestamp) {
				earliestTimestamp = timestamp;
			}
			if (timestamp > latestTimestamp) {
				latestTimestamp = timestamp;
			}
		}

		// Save the time extent of the data points
		this->startTime = earliestTimestamp;
		this->endTime = latestTimestamp;
	}
}

JsonReader::~JsonReader() {
	this->clearCache();
}

void JsonReader::indexDataPoints(rapidjson::Value& jsonValue, int depth) {
	for (rapidjson::SizeType i = 0; i < jsonValue.Size(); i++) {
		// Validate the data point's content before using it
		JsonValidator::validateChildElement(jsonValue[i], this->dataType, this->filePath);

		DataPoint* dataPoint;

		// Initialize the new data point depending on the data type
		switch (this->dataType) {
		case Data::CITIES:
			dataPoint = new CityDataPoint(
			    Coordinate(
			        jsonValue[i]["latitude"].GetDouble(),
			        jsonValue[i]["longitude"].GetDouble()
			    ),
			    depth,
			    jsonValue[i]["name"].GetString()
			);
			break;
		case Data::FLIGHTS: {
			Coordinate startPosition(jsonValue[i]["startPosition"]["latitude"].GetDouble(),
			                         jsonValue[i]["startPosition"]["longitude"].GetDouble());
			Coordinate endPosition(jsonValue[i]["endPosition"]["latitude"].GetDouble(),
			                       jsonValue[i]["endPosition"]["longitude"].GetDouble());
			double flightLength = calculateDistance(startPosition, endPosition);

			dataPoint = new FlightDataPoint(
			    startPosition,
			    depth,
			    endPosition,
			    jsonValue[i]["airline"].GetString(),
			    jsonValue[i]["startPosition"]["airportCode"].GetString(),
			    jsonValue[i]["endPosition"]["airportCode"].GetString(),
			    flightLength
			);
			break;
		}
		case Data::TWEETS:
			dataPoint = new TweetDataPoint(
			    Coordinate(
			        jsonValue[i]["latitude"].GetDouble(),
			        jsonValue[i]["longitude"].GetDouble()
			    ),
			    depth,
			    jsonValue[i]["timestamp"].GetInt(),
			    jsonValue[i]["author"].GetString(),
			    jsonValue[i]["content"].GetString(),
                jsonValue[i]["numberOfRetweets"].GetInt()
			);
			break;
		case Data::PRECIPITATION: {
			PrecipitationDataPoint::PrecipitationType precipitationType
			    = PrecipitationDataPoint::NONE;

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
			break;
		}
		case Data::TEMPERATURE:
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
		case Data::WIND:
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
		case Data::CLOUD_COVERAGE:
			dataPoint = new CloudCoverageDataPoint(
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

Data::Type JsonReader::getDataType() const {
	return this->dataType;
}

bool JsonReader::hasTemporalData() const {
	return temporal;
}

int JsonReader::getAmountOfTimeSteps() const {
	return (this->endTime - this->startTime) / this->timeResolution;
}

int JsonReader::getTimeResolution() const {
	if (this->hasTemporalData()) {
		return this->timeResolution;
	} else {
		return 0;
	}
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
		if (this->hasTemporalData()) {
			for (float currentTime = 0.0f; currentTime <= 1.0f;
			        currentTime += (this->endTime - this->startTime) / (this->timeResolution * 1.0f)) {
				this->getVtkDataSet(i, currentTime);
			}
		} else {
			this->getVtkDataSet(i);
		}
	}
}

void JsonReader::clearCache() {
	this->nonTemporalCache.clear();
	this->temporalCache.clear();
}

vtkSmartPointer<vtkPolyData> JsonReader::getVtkDataSet(int zoomLevel) {
	return this->getVtkDataSet(zoomLevel, 0, false);
}

vtkSmartPointer<vtkPolyData> JsonReader::getVtkDataSet(int zoomLevel, int time) {
	return this->getVtkDataSet(zoomLevel, time, true);
}

vtkSmartPointer<vtkPolyData> JsonReader::getVtkDataSet(int zoomLevel, int time, bool temporal) {
	bool verifiedTemporal = this->hasTemporalData() && temporal;

	// If possible, retrieve the data set from the cache
	if (this->cachingEnabled) {
		if (verifiedTemporal) {
			if (this->temporalCache.contains(zoomLevel)) {
				if (this->temporalCache.value(zoomLevel).contains(time)) {
					return this->temporalCache.value(zoomLevel).value(time);
				}
			}
		} else {
			if (this->nonTemporalCache.contains(zoomLevel)) {
				return this->nonTemporalCache.value(zoomLevel);
			}
		}
	}

	vtkSmartPointer<vtkPolyData> dataSet;

	if (verifiedTemporal) {
		dataSet = PolyDataSetHelper::getPolyDataFromDataPoints(
		              this->pointDataSet,
		              zoomLevel,
		              this->dataType,
		              this->timeResolution,
		              time,
		              this->startTime
		          );
	} else {
		dataSet = PolyDataSetHelper::getPolyDataFromDataPoints(
		              this->pointDataSet,
		              zoomLevel,
		              this->dataType
		          );
	}

	// Save the data set to the cache if desired
	if (this->cachingEnabled) {
		if (verifiedTemporal) {
			if (!this->temporalCache.contains(zoomLevel)) {
				QMap<int, vtkSmartPointer<vtkPolyData>> zoomLevelCache;
				this->temporalCache[zoomLevel] = zoomLevelCache;
			}

			this->temporalCache[zoomLevel][time] = dataSet;
		} else {
			this->nonTemporalCache[zoomLevel] = dataSet;
		}
	}

	return dataSet;
}