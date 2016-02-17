#include <Filter/TemporalAggregationFilter/DataAggregator.hpp>

#include <Filter/TemporalAggregationFilter/PrecipitationAggregationValue.hpp>
#include <Filter/TemporalAggregationFilter/TemperatureAggregationValue.hpp>
#include <Filter/TemporalAggregationFilter/WindAggregationValue.hpp>
#include <Filter/TemporalAggregationFilter/CloudCoverageAggregationValue.hpp>

#include <vtkDataArray.h>
#include <vtkTypeFloat32Array.h>

DataAggregator::DataAggregator() : lastTimeIndex(0) { }

DataAggregator::~DataAggregator() {
    qDeleteAll(this->aggregatedData);
    this->aggregatedData.clear();
}

void DataAggregator::setDataSetAttributes(Data::Type dataType, int timeResolution) {
    this->dataType = dataType;
    this->timeResolution = timeResolution;
}

void DataAggregator::addPointData(int pointIndex, PointCoordinates coordinates, vtkSmartPointer<vtkPointData> pointData) {
    int currentTimeStep = this->lastTimeIndex;
    
    switch (this->dataType) {
        case Data::PRECIPITATION: {
            vtkSmartPointer<vtkDataArray> abstractPrecipitationRateArray = pointData->GetArray("precipitationRates");
            vtkSmartPointer<vtkTypeFloat32Array> precipitationRateArray = vtkTypeFloat32Array::SafeDownCast(abstractPrecipitationRateArray);
            double currentPrecipitationRate = precipitationRateArray->GetValue(pointIndex);
        
            if (this->aggregatedData.contains(coordinates)) {
                // The point has already been looked at before
                PrecipitationAggregationValue* currentValue = static_cast<PrecipitationAggregationValue*>(this->aggregatedData.value(coordinates));
            
                // Accumulate the precipitation rates over time, therefore getting the total amount of precipitation.
                // To do this, two assumptions are made:
                // 1. The precipitation amount at the beginning of the accumulation is zero.
                // 2. A data point constantly emits precipitation with its precipitation rate until new precipitation rate data is available.
                currentValue->setAccumulatedPrecipitation(currentValue->getAccumulatedPrecipitation() + ((1.0 * (currentTimeStep - currentValue->getTimeIndex()) * this->timeResolution) * currentValue->getLastPrecipitationRate()));
                currentValue->setTimeIndex(currentTimeStep);
                currentValue->setLastPrecipitationRate(currentPrecipitationRate);
            } else {
                // This is the first time a point with these coordinates shows up
                PrecipitationAggregationValue* newValue = new PrecipitationAggregationValue();
                newValue->setTimeIndex(currentTimeStep);
                newValue->setLastPrecipitationRate(currentPrecipitationRate);
                this->aggregatedData.insert(coordinates, newValue);
            }
            break; }
        case Data::TEMPERATURE: {
            vtkSmartPointer<vtkDataArray> abstractTemperatureArray = pointData->GetArray("temperatures");
            vtkSmartPointer<vtkTypeFloat32Array> temperatureArray = vtkTypeFloat32Array::SafeDownCast(abstractTemperatureArray);
            double currentTemperature = temperatureArray->GetValue(pointIndex);
        
            if (this->aggregatedData.contains(coordinates)) {
                // The point has already been looked at before
                TemperatureAggregationValue* currentValue = static_cast<TemperatureAggregationValue*>(this->aggregatedData.value(coordinates));
            
                // Calculate the arithmetric mean with the cumulative moving average method.
                // This is a bit more costly than the naive way to calculate the average but prevents huge numbers from showing up while summing up all values.
                currentValue->setAverageTemperature((currentTemperature + (currentValue->getTimeIndex() * currentValue->getAverageTemperature())) / (currentValue->getTimeIndex() * 1.0 + 1));
                currentValue->setTimeIndex(currentValue->getTimeIndex() + 1);
            } else {
                // This is the first time a point with these coordinates shows up
                TemperatureAggregationValue* newValue = new TemperatureAggregationValue();
                newValue->setAverageTemperature(currentTemperature);
                this->aggregatedData.insert(coordinates, newValue);
            }
            break; }
        case Data::WIND: {
            vtkSmartPointer<vtkDataArray> abstractVelocitiesArray = pointData->GetArray("speeds");
            vtkSmartPointer<vtkTypeFloat32Array> velocitiesArray = vtkTypeFloat32Array::SafeDownCast(abstractVelocitiesArray);
            double currentVelocity = velocitiesArray->GetValue(pointIndex);
            
            vtkSmartPointer<vtkDataArray> abstractBearingsArray = pointData->GetArray("directions");
            vtkSmartPointer<vtkTypeFloat32Array> bearingsArray = vtkTypeFloat32Array::SafeDownCast(abstractBearingsArray);
            double currentBearing = bearingsArray->GetValue(pointIndex);
        
            if (this->aggregatedData.contains(coordinates)) {
                // The point has already been looked at before
                WindAggregationValue* currentValue = static_cast<WindAggregationValue*>(this->aggregatedData.value(coordinates));
            
                // Calculate the arithmetric mean with the cumulative moving average method.
                // This is a bit more costly than the naive way to calculate the average but prevents huge numbers from showing up while summing up all values.
                currentValue->setAverageVelocity((currentVelocity + (currentValue->getTimeIndex() * currentValue->getAverageVelocity())) / (currentValue->getTimeIndex() * 1.0 + 1));                
                currentValue->setAverageBearing((currentBearing + (currentValue->getTimeIndex() * currentValue->getAverageBearing())) / (currentValue->getTimeIndex() * 1.0 + 1));
                currentValue->setTimeIndex(currentValue->getTimeIndex() + 1);
            } else {
                // This is the first time a point with these coordinates shows up
                WindAggregationValue* newValue = new WindAggregationValue();
                newValue->setAverageVelocity(currentVelocity);
                newValue->setAverageBearing(currentBearing);
                this->aggregatedData.insert(coordinates, newValue);
            }
            break; }
        case Data::CLOUD_COVERAGE: {
            vtkSmartPointer<vtkDataArray> abstractCoverageArray = pointData->GetArray("cloudCovers");
            vtkSmartPointer<vtkTypeFloat32Array> coverageArray = vtkTypeFloat32Array::SafeDownCast(abstractCoverageArray);
            double currentCoverage = coverageArray->GetValue(pointIndex);
        
            if (this->aggregatedData.contains(coordinates)) {
                // The point has already been looked at before
                CloudCoverageAggregationValue* currentValue = static_cast<CloudCoverageAggregationValue*>(this->aggregatedData.value(coordinates));
            
                // Calculate the arithmetric mean with the cumulative moving average method.
                // This is a bit more costly than the naive way to calculate the average but prevents huge numbers from showing up while summing up all values.
                currentValue->setAverageCloudCoverage((currentCoverage + (currentValue->getTimeIndex() * currentValue->getAverageCloudCoverage())) / (currentValue->getTimeIndex() * 1.0 + 1));
                currentValue->setTimeIndex(currentValue->getTimeIndex() + 1);
            } else {
                // This is the first time a point with these coordinates shows up
                CloudCoverageAggregationValue* newValue = new CloudCoverageAggregationValue();
                newValue->setAverageCloudCoverage(currentCoverage);
                this->aggregatedData.insert(coordinates, newValue);
            }
            break; }
        default:
            break;
    }
    
    this->lastTimeIndex++;
}