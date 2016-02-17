#include <Filter/TemporalAggregationFilter/DataAggregator.hpp>

#include <Filter/TemporalAggregationFilter/PrecipitationAggregationValue.hpp>
#include <Filter/TemporalAggregationFilter/TemperatureAggregationValue.hpp>
#include <Filter/TemporalAggregationFilter/WindAggregationValue.hpp>
#include <Filter/TemporalAggregationFilter/CloudCoverageAggregationValue.hpp>

#include <vtkPoints.h>
#include <vtkCellArray.h>
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

vtkSmartPointer<vtkPolyData> DataAggregator::getPolyData() {
    vtkSmartPointer<vtkPolyData> dataSet = vtkSmartPointer<vtkPolyData>::New();
    
    // Create the content of the output poly data object
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
	vertices->Allocate(vertices->EstimateSize(1, this->aggregatedData.size()));
	vertices->InsertNextCell(this->aggregatedData.size());
    
    switch (this->dataType) {
        case Data::PRECIPITATION: {
            // Create the array that will hold the aggregated precipitation amounts
            vtkSmartPointer<vtkTypeFloat32Array> aggregatedPrecipitationAmounts = vtkSmartPointer<vtkTypeFloat32Array>::New();
            aggregatedPrecipitationAmounts->SetNumberOfComponents(1);
            aggregatedPrecipitationAmounts->SetNumberOfTuples(this->aggregatedData.size());
            aggregatedPrecipitationAmounts->SetName("Accumulated Precipitation Amounts");
            
            // Iterate over all points in the aggregated data
            QMap<PointCoordinates, AggregationValue*>::iterator i;
            int tupleNumber = 0;
            for (i = this->aggregatedData.begin(); i != this->aggregatedData.end(); ++i) {
                PrecipitationAggregationValue* currentValue = static_cast<PrecipitationAggregationValue*>(i.value());
                
                // Add the point's coordinates
                vertices->InsertCellPoint(points->InsertNextPoint(i.key().getX(), i.key().getY(), i.key().getZ()));
                
                // Add the point's accumulated precipitation amounts
                double aggregatedPrecipitationAmount[1] = {
    				(double) currentValue->getAccumulatedPrecipitation()
    			};
                aggregatedPrecipitationAmounts->SetTuple(tupleNumber, aggregatedPrecipitationAmount);
            }
            
            dataSet->GetPointData()->AddArray(aggregatedPrecipitationAmounts);
            break;
        }
        case Data::TEMPERATURE: {
            // Create the array that will hold the aggregated temperatures
            vtkSmartPointer<vtkTypeFloat32Array> aggregatedTemperatures = vtkSmartPointer<vtkTypeFloat32Array>::New();
            aggregatedTemperatures->SetNumberOfComponents(1);
            aggregatedTemperatures->SetNumberOfTuples(this->aggregatedData.size());
            aggregatedTemperatures->SetName("Average Temperatures");
            
            // Iterate over all points in the aggregated data
            QMap<PointCoordinates, AggregationValue*>::iterator i;
            int tupleNumber = 0;
            for (i = this->aggregatedData.begin(); i != this->aggregatedData.end(); ++i) {
                TemperatureAggregationValue* currentValue = static_cast<TemperatureAggregationValue*>(i.value());
                
                // Add the point's coordinates
                vertices->InsertCellPoint(points->InsertNextPoint(i.key().getX(), i.key().getY(), i.key().getZ()));
                
                // Add the point's average temperatures
                double aggregatedTemperature[1] = {
    				(double) currentValue->getAverageTemperature()
    			};
                aggregatedTemperatures->SetTuple(tupleNumber, aggregatedTemperature);
            }
            
            dataSet->GetPointData()->AddArray(aggregatedTemperatures);
            break;
        }
        case Data::WIND: {

            break;
        }
        case Data::CLOUD_COVERAGE: {

            break;
        }
        default:
            break;
    }

	// Assign the created point set to the output object
	dataSet->SetPoints(points);
	dataSet->SetVerts(vertices);
    
    return dataSet;
}

void DataAggregator::finishAggregation() {
    if (this->dataType == Data::PRECIPITATION) {
        // The accumulation of precipitation amount must be finished for data points that had no new data available in the time step last accumulated
        QMap<PointCoordinates, AggregationValue*>::iterator i;
        for (i = this->aggregatedData.begin(); i != this->aggregatedData.end(); ++i) {
            PrecipitationAggregationValue* currentValue = static_cast<PrecipitationAggregationValue*>(i.value());
            
            currentValue->setAccumulatedPrecipitation(currentValue->getAccumulatedPrecipitation() + ((1.0 * (this->lastTimeIndex - currentValue->getTimeIndex()) * this->timeResolution) * currentValue->getLastPrecipitationRate()));
            currentValue->setTimeIndex(this->lastTimeIndex);
        }
    }
}