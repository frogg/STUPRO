#include <Filter/TemporalInterpolationFilter.h>

#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkCellArray.h>

#include <Filter/TemporalInterpolationFilter/PrecipitationInterpolationValue.hpp>
#include <Filter/TemporalInterpolationFilter/TemperatureInterpolationValue.hpp>
#include <Filter/TemporalInterpolationFilter/WindInterpolationValue.hpp>
#include <Filter/TemporalInterpolationFilter/CloudCoverageInterpolationValue.hpp>
#include <Filter/TemporalInterpolationFilter/TwitterInterpolationValue.hpp>
#include <Utils/Misc/MakeUnique.hpp>
#include <Utils/Math/Functions.hpp>

vtkStandardNewMacro(TemporalInterpolationFilter);

const QList<Data::Type> TemporalInterpolationFilter::SUPPORTED_DATA_TYPES = QList<Data::Type>() <<
        Data::PRECIPITATION << Data::TEMPERATURE << Data::WIND << Data::CLOUD_COVERAGE << Data::TWEETS;

TemporalInterpolationFilter::TemporalInterpolationFilter() : preprocessed(false), error(false), currentTimeStep(0) {
	this->SetNumberOfInputPorts(1);
	this->SetNumberOfOutputPorts(1);
}

TemporalInterpolationFilter::~TemporalInterpolationFilter() {
    for(int i = 0; i < this->pointData.size(); i++){
        qDeleteAll(this->pointData[i]);
    }
}

void TemporalInterpolationFilter::fail(QString message) {
	vtkErrorMacro( << message.toStdString());
	this->error = true;
}

void TemporalInterpolationFilter::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);

	os << indent << "Temporal data point interpolation, Kronos Project" << endl;
}

int TemporalInterpolationFilter::FillInputPortInformation(int port, vtkInformation* info) {
	if (port == 0) {
		info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
		info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
	}

	return 1;
}

int TemporalInterpolationFilter::FillOutputPortInformation(int port, vtkInformation* info) {
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
	return 1;
}

int TemporalInterpolationFilter::RequestInformation (
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) {
	if (this->error) {
		return 0;
	}

	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);

	if (inInfo->Has(Data::VTK_DATA_TYPE())) {
		this->dataType = static_cast<Data::Type>(inInfo->Get(Data::VTK_DATA_TYPE()));
	} else {
		this->fail("This filter only works with data read by the Kronos reader.");
		return 0;
	}

	// Overkill code for creating a comma-separated string with the names of all supported data types
	QString supportedTypes = "";
	int amountOfSupportedDataTypes = TemporalInterpolationFilter::SUPPORTED_DATA_TYPES.size();
	if (amountOfSupportedDataTypes == 1) {
		supportedTypes.append(Data::getDataTypeName(TemporalInterpolationFilter::SUPPORTED_DATA_TYPES.value(
		                          0)));
	} else if (amountOfSupportedDataTypes > 1) {
		for (int i = 0; i < amountOfSupportedDataTypes - 2; i++) {
			supportedTypes.append(Data::getDataTypeName(TemporalInterpolationFilter::SUPPORTED_DATA_TYPES.value(
			                          i)));
			if (i < amountOfSupportedDataTypes - 3) {
				supportedTypes.append(", ");
			}
		}
		supportedTypes.append(" and ").append(Data::getDataTypeName(
		        TemporalInterpolationFilter::SUPPORTED_DATA_TYPES.value(amountOfSupportedDataTypes - 1)));
	}

	if (!TemporalInterpolationFilter::SUPPORTED_DATA_TYPES.contains(this->dataType)) {
		this->fail(QString("This filter only supports %1 data, but the input contains %2 data.").arg(
		               supportedTypes, Data::getDataTypeName(this->dataType)));
		return 0;
	}
    
    // TODO: Check the data's state for compatibility

	return 1;
}

int TemporalInterpolationFilter::RequestData(
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) {
	if (this->error) {
		return 0;
	}
    
    vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation* outInfo = outputVector->GetInformationObject(0);

    vtkPolyData* input = vtkPolyData::GetData(inInfo);
    vtkPolyData* output = vtkPolyData::GetData(outInfo);
    
    if (this->hasPreprocessed()) {
        output->DeepCopy(this->getOutputPolyData(outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP())));
        request->Set(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING(), 0);
        return 1;
    }

	if (this->currentTimeStep == 0) {
		this->SetProgressText("Interpolating data...");
		this->SetProgress(0.0);
	}

    this->storeTimestepData(this->currentTimeStep, input);
    
	if (this->currentTimeStep < inInfo->Length(vtkStreamingDemandDrivenPipeline::TIME_STEPS())) {
		// There are still time steps left, continue on
		this->currentTimeStep++;

		// Update the progress
		double progress = (double) (this->currentTimeStep / (1.0 * inInfo->Length(
		                                vtkStreamingDemandDrivenPipeline::TIME_STEPS())));
		this->UpdateProgress(progress);
	} else {
		// The data points have been iterated
		request->Remove(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING());
		this->currentTimeStep = 0;
		this->SetProgressText("");
		this->SetProgress(1.0);
        this->fillTimesteps();
        this->preprocessed = true;
	}
    
    request->Set(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING(), 1);
	return 1;
}

vtkSmartPointer<vtkPolyData> TemporalInterpolationFilter::getOutputPolyData(double time) {
    vtkSmartPointer<vtkPolyData> dataSet = vtkSmartPointer<vtkPolyData>::New();
    
    // Interpolate between the two adjacent time steps
    QMap<PointCoordinates, InterpolationValue*> interpolatedData;
    float factorA = time - (int) time;
    float factorB = 1 - factorA;
    
    for (QMap<PointCoordinates, InterpolationValue*>::iterator iterator = this->pointData[(int) time].begin();
	        iterator != this->pointData[(int) time].end(); ++iterator) {
        interpolatedData.insert(iterator.key(), this->interpolateDataPoint(this->pointData[((int) time)][iterator.key()], this->pointData[((int) time) + 1][iterator.key()], factorA, factorB));
    }

    // Create the content of the output poly data object
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkCellArray* verts = vtkCellArray::New();
	verts->Allocate(verts->EstimateSize(1, interpolatedData.size()));
	verts->InsertNextCell(interpolatedData.size());

	vtkIdType numberOfTuples[1];
	numberOfTuples[0] = interpolatedData.size();

    // Add the common priority and timestamp arrays
	vtkSmartPointer<vtkIntArray> priorities = vtkSmartPointer<vtkIntArray>::New();
	priorities->SetName("priorities");
	priorities->SetNumberOfComponents(1);
	priorities->SetNumberOfTuples(*numberOfTuples);

	vtkSmartPointer<vtkIntArray> timestamps = vtkSmartPointer<vtkIntArray>::New();
	timestamps->SetName("timestamps");
	timestamps->SetNumberOfComponents(1);
	timestamps->SetNumberOfTuples(*numberOfTuples);

	// The current tuple index in the iteration
	int tupleNumber = 0;

	// Iterate through all data points, add their coordinates as new points and fill the
	// array of priority and timestamp values.
	for (QMap<PointCoordinates, InterpolationValue*>::iterator iterator = interpolatedData.begin();
	        iterator != interpolatedData.end(); ++iterator) {
		verts->InsertCellPoint(points->InsertNextPoint(
		                           iterator.key().getX(),
		                           iterator.key().getY(),
		                           iterator.key().getZ()
		                       ));

		double priority[1] = {(double) iterator.value()->getPriority()};
		priorities->SetTuple(tupleNumber, priority);

		double timestamp[1] = {(double) iterator.value()->getTimestamp()};
		timestamps->SetTuple(tupleNumber, timestamp);

		tupleNumber++;
	}
    
    dataSet->GetPointData()->AddArray(timestamps);
    dataSet->GetPointData()->AddArray(priorities);
    
    switch (this->dataType) {
        case Data::TEMPERATURE:{
            vtkSmartPointer<vtkFloatArray> temperatures
    		    = vtkSmartPointer<vtkFloatArray>::New();
    		temperatures->SetNumberOfComponents(1);
    		temperatures->SetNumberOfTuples(interpolatedData.size());
    		temperatures->SetName("temperatures");

    		int tupleNumber = 0;
    		for (QMap<PointCoordinates, InterpolationValue*>::iterator iterator = interpolatedData.begin();
    		        iterator != interpolatedData.end(); ++iterator) {
    			const TemperatureInterpolationValue* dataPoint
    			    = static_cast<const TemperatureInterpolationValue*>(iterator.value());

    			double temperature[1] = {
    				(double) dataPoint->getTemperature()
    			};
    			temperatures->SetTuple(tupleNumber, temperature);

    			tupleNumber++;
    		}

    		dataSet->GetPointData()->AddArray(temperatures);
            
            break;
        }
        case Data::TWEETS: {
            vtkSmartPointer<vtkFloatArray> densities
    		    = vtkSmartPointer<vtkFloatArray>::New();
    		densities->SetNumberOfComponents(1);
    		densities->SetNumberOfTuples(interpolatedData.size());
    		densities->SetName("density");

    		int tupleNumber = 0;
    		for (QMap<PointCoordinates, InterpolationValue*>::iterator iterator = interpolatedData.begin();
    		        iterator != interpolatedData.end(); ++iterator) {
    			const TwitterInterpolationValue* dataPoint
    			    = static_cast<const TwitterInterpolationValue*>(iterator.value());

    			double density[1] = {
    				(double) dataPoint->getDensity()
    			};
    			densities->SetTuple(tupleNumber, density);

    			tupleNumber++;
    		}

    		dataSet->GetPointData()->AddArray(densities);
            
            break;
        }
        case Data::PRECIPITATION: {
            vtkSmartPointer<vtkFloatArray> precipitationRates
    		    = vtkSmartPointer<vtkFloatArray>::New();
    		precipitationRates->SetNumberOfComponents(1);
    		precipitationRates->SetNumberOfTuples(interpolatedData.size());
    		precipitationRates->SetName("precipitationRates");
            
            vtkSmartPointer<vtkIntArray> precipitationTypes
    		    = vtkSmartPointer<vtkIntArray>::New();
    		precipitationTypes->SetNumberOfComponents(1);
    		precipitationTypes->SetNumberOfTuples(interpolatedData.size());
    		precipitationTypes->SetName("precipitationTypes");

    		int tupleNumber = 0;
    		for (QMap<PointCoordinates, InterpolationValue*>::iterator iterator = interpolatedData.begin();
    		        iterator != interpolatedData.end(); ++iterator) {
    			const PrecipitationInterpolationValue* dataPoint
    			    = static_cast<const PrecipitationInterpolationValue*>(iterator.value());

    			double precipitationRate[1] = {
    				(double) dataPoint->getPrecipitationRate()
    			};
    			precipitationRates->SetTuple(tupleNumber, precipitationRate);
                
                double precipitationType[1] = {
    				(double) dataPoint->getPrecipitationType()
    			};
    			precipitationTypes->SetTuple(tupleNumber, precipitationType);

    			tupleNumber++;
    		}

    		dataSet->GetPointData()->AddArray(precipitationRates);
            dataSet->GetPointData()->AddArray(precipitationTypes);
            
            break;
        }
        case Data::WIND: {
            vtkSmartPointer<vtkFloatArray> windDirections
    		    = vtkSmartPointer<vtkFloatArray>::New();
    		windDirections->SetNumberOfComponents(1);
    		windDirections->SetNumberOfTuples(interpolatedData.size());
    		windDirections->SetName("directions");
            
            vtkSmartPointer<vtkFloatArray> windSpeeds
    		    = vtkSmartPointer<vtkFloatArray>::New();
    		windSpeeds->SetNumberOfComponents(1);
    		windSpeeds->SetNumberOfTuples(interpolatedData.size());
    		windSpeeds->SetName("speeds");
            
            vtkSmartPointer<vtkFloatArray> velocities
    		    = vtkSmartPointer<vtkFloatArray>::New();
    		velocities->SetNumberOfComponents(3);
    		velocities->SetNumberOfTuples(interpolatedData.size());
    		velocities->SetName("velocity");

    		int tupleNumber = 0;
    		for (QMap<PointCoordinates, InterpolationValue*>::iterator iterator = interpolatedData.begin();
    		        iterator != interpolatedData.end(); ++iterator) {
    			const WindInterpolationValue* dataPoint
    			    = static_cast<const WindInterpolationValue*>(iterator.value());

    			double windDirection[1] = {
    				(double) dataPoint->getBearing()
    			};
    			windDirections->SetTuple(tupleNumber, windDirection);
                
                double windSpeed[1] = {
    				(double) dataPoint->getSpeed()
    			};
    			windSpeeds->SetTuple(tupleNumber, windSpeed);
                
                float windBearingRadian = toRadians(dataPoint->getBearing());
    			double velocity[3] = {
    				(double) dataPoint->getSpeed() * sin(windBearingRadian),
    				(double) dataPoint->getSpeed() * cos(windBearingRadian),
    				0.0
    			};
    			velocities->SetTuple(tupleNumber, velocity);

    			tupleNumber++;
    		}

    		dataSet->GetPointData()->AddArray(windSpeeds);
            dataSet->GetPointData()->AddArray(windDirections);
            dataSet->GetPointData()->AddArray(velocities);
            
            break;
        }
        case Data::CLOUD_COVERAGE: {
            vtkSmartPointer<vtkFloatArray> cloudCoverageArray
    		    = vtkSmartPointer<vtkFloatArray>::New();
    		cloudCoverageArray->SetNumberOfComponents(1);
    		cloudCoverageArray->SetNumberOfTuples(interpolatedData.size());
    		cloudCoverageArray->SetName("cloudCovers");

    		int tupleNumber = 0;
    		for (QMap<PointCoordinates, InterpolationValue*>::iterator iterator = interpolatedData.begin();
    		        iterator != interpolatedData.end(); ++iterator) {
    			const CloudCoverageInterpolationValue* dataPoint
    			    = static_cast<const CloudCoverageInterpolationValue*>(iterator.value());

    			double cloudCoverage[1] = {
    				(double) dataPoint->getCloudCoverage()
    			};
    			cloudCoverageArray->SetTuple(tupleNumber, cloudCoverage);

    			tupleNumber++;
    		}

    		dataSet->GetPointData()->AddArray(cloudCoverageArray);
            
            break;
        }
        default: {
            this->fail("The data type of this filter seems to be invalid.");
            return nullptr;
            break;
        }
    }

	// Assign points and vertices to the output data set
	dataSet->SetPoints(points);
	dataSet->SetVerts(verts);
    
    qDeleteAll(interpolatedData);

    return dataSet;
}

void TemporalInterpolationFilter::storeTimestepData(int timestep, vtkPolyData *inputData) {
    QMap<PointCoordinates, InterpolationValue*> timestepData;
    
    for (int i = 0; i < inputData->GetNumberOfPoints(); i++) {
        double coordinates[3];
        inputData->GetPoint(i, coordinates);
        
        PointCoordinates currentCoordinates(coordinates[0], coordinates[1], coordinates[2]);
        timestepData.insert(currentCoordinates, this->createDataPoint(i, inputData));
    }

    this->pointData.insert(timestep, timestepData);
}

InterpolationValue* TemporalInterpolationFilter::createDataPoint(int pointIndex, vtkPolyData *inputData) {
    vtkSmartPointer<vtkIntArray> priorityArray = vtkIntArray::SafeDownCast(inputData->GetPointData()->GetArray("priorities"));
    int priority = priorityArray->GetValue(pointIndex);
    
    vtkSmartPointer<vtkIntArray> timestampArray = vtkIntArray::SafeDownCast(inputData->GetPointData()->GetArray("timestamps"));
    int timestamp = timestampArray->GetValue(pointIndex);
    
    switch (this->dataType) {
        case Data::TEMPERATURE:{
            vtkSmartPointer<vtkFloatArray> temperatureArray = vtkFloatArray::SafeDownCast(inputData->GetPointData()->GetArray("temperatures"));
            return new TemperatureInterpolationValue(priority, timestamp, temperatureArray->GetValue(pointIndex));
            break;
        }
        case Data::TWEETS: {
            vtkSmartPointer<vtkFloatArray> densityArray = vtkFloatArray::SafeDownCast(inputData->GetPointData()->GetArray("density"));
            return new TwitterInterpolationValue(priority, timestamp, densityArray->GetValue(pointIndex));
            break;
        }
        case Data::PRECIPITATION: {
            vtkSmartPointer<vtkFloatArray> precipitationRateArray = vtkFloatArray::SafeDownCast(inputData->GetPointData()->GetArray("precipitationRates"));
            vtkSmartPointer<vtkIntArray> precipitationTypeArray = vtkIntArray::SafeDownCast(inputData->GetPointData()->GetArray("precipitationTypes"));
            return new PrecipitationInterpolationValue(priority, timestamp, precipitationRateArray->GetValue(pointIndex), static_cast<PrecipitationDataPoint::PrecipitationType>(precipitationTypeArray->GetValue(pointIndex)));
            break;
        }
        case Data::WIND: {
            vtkSmartPointer<vtkFloatArray> bearingArray = vtkFloatArray::SafeDownCast(inputData->GetPointData()->GetArray("directions"));
            vtkSmartPointer<vtkFloatArray> velocityArray = vtkFloatArray::SafeDownCast(inputData->GetPointData()->GetArray("speeds"));
            return new WindInterpolationValue(priority, timestamp, bearingArray->GetValue(pointIndex), velocityArray->GetValue(pointIndex));
            break;
        }
        case Data::CLOUD_COVERAGE: {
            vtkSmartPointer<vtkFloatArray> cloudCoverageArray = vtkFloatArray::SafeDownCast(inputData->GetPointData()->GetArray("cloudCovers"));
            return new CloudCoverageInterpolationValue(priority, timestamp, cloudCoverageArray->GetValue(pointIndex));
            break;
        }
        default: {
            this->fail("The data type of this filter seems to be invalid.");
            return new InterpolationValue();
            break;
        }
    }
}

void TemporalInterpolationFilter::fillTimesteps() {
    // First of all, fill in the first and the last time step with points, whose information only are available in the middle of the time span. This groundwork is necessary to enable easier linear interpolation of intermediate points using the algorithm below.
    QMap<PointCoordinates, InterpolationValue*> firstTimeStep;
    QMap<PointCoordinates, InterpolationValue*> lastTimeStep;
    
    for (int t = 0; t < this->pointData.size(); t++) {
        // Iterate through all points in all time steps
        QMap<PointCoordinates, InterpolationValue*>::const_iterator i = this->pointData[t].constBegin();
        while (i != this->pointData[t].constEnd()) {
            // Add newly found points to the first time step only if they are not there yet, therefore keeping the chronologically first information
            if (!firstTimeStep.contains(i.key())) {
                firstTimeStep.insert(i.key(), i.value());
            }
            
            // Always add newly found points to the last time step, therefore keeping the chronologically last information
            lastTimeStep[i.key()] = i.value();
            
            ++i;
        }
    }
    
    this->pointData[0] = firstTimeStep;
    this->pointData[this->pointData.size() - 1] = lastTimeStep;
    
    // After this has been done, the real interpolation can start. It will fill all remaining gaps in the time steps with linearly interpolated data.
    QList<PointCoordinates> allPoints = this->pointData[0].keys();
    
    for (int i = 0; i < allPoints.size(); i++) {
        int leftIndex = 0;
        int rightIndex = 0;
        while (leftIndex < this->pointData.size() - 1) {
            rightIndex++;
            
            if (this->pointData[rightIndex].contains(allPoints[i])) {
                for (int j = leftIndex; j <= rightIndex; j++) {
                    float factorA = float(j - leftIndex) / float(rightIndex - leftIndex);
                    float factorB = 1 - factorA;
                    this->pointData[j].insert(allPoints[i], this->interpolateDataPoint(this->pointData[leftIndex][allPoints[i]], this->pointData[rightIndex][allPoints[i]], factorA, factorB));
                }
                
                leftIndex = rightIndex;
            }
        }        
    }
}

InterpolationValue* TemporalInterpolationFilter::interpolateDataPoint(InterpolationValue *left, InterpolationValue *right, float factorA, float factorB) {
    int priority;
    if (factorA >= 0.5) {
        priority = right->getPriority();
    } else {
        priority = left->getPriority();
    }
    
    int interpolatedTimestamp = int(factorB * left->getTimestamp() + factorA * right->getTimestamp());
    
    switch (this->dataType) {
        case Data::TEMPERATURE:{
            TemperatureInterpolationValue* leftValue = static_cast<TemperatureInterpolationValue*>(left);
            TemperatureInterpolationValue* rightValue = static_cast<TemperatureInterpolationValue*>(right);
            
            float interpolatedTemperature = factorB * leftValue->getTemperature() + factorA * rightValue->getTemperature();
            
            return new TemperatureInterpolationValue(priority, interpolatedTimestamp, interpolatedTemperature);
            break;
        }
        case Data::TWEETS: {
            TwitterInterpolationValue* leftValue = static_cast<TwitterInterpolationValue*>(left);
            TwitterInterpolationValue* rightValue = static_cast<TwitterInterpolationValue*>(right);
            
            float interpolatedDensity = factorB * leftValue->getDensity() + factorA * rightValue->getDensity();
            
            return new TwitterInterpolationValue(priority, interpolatedTimestamp, interpolatedDensity);
            break;
        }
        case Data::PRECIPITATION: {
            PrecipitationInterpolationValue* leftValue = static_cast<PrecipitationInterpolationValue*>(left);
            PrecipitationInterpolationValue* rightValue = static_cast<PrecipitationInterpolationValue*>(right);
            
            PrecipitationDataPoint::PrecipitationType precipitationType;
            if(factorA >= 0.5){
                precipitationType = rightValue->getPrecipitationType();
            }else{
                precipitationType = leftValue->getPrecipitationType();
            }
            
            float interpolatedPrecipitationRate = factorB * leftValue->getPrecipitationRate() + factorA * rightValue->getPrecipitationRate();
            
            return new PrecipitationInterpolationValue(priority, interpolatedTimestamp, interpolatedPrecipitationRate, precipitationType);
            break;
        }
        case Data::WIND: {
            WindInterpolationValue* leftValue = static_cast<WindInterpolationValue*>(left);
            WindInterpolationValue* rightValue = static_cast<WindInterpolationValue*>(right);
            
            float interpolatedBearing = factorB * leftValue->getBearing() + factorA * rightValue->getBearing();
            float interpolatedSpeed = factorB * leftValue->getSpeed() + factorA * rightValue->getSpeed();
           
            return new WindInterpolationValue(priority, interpolatedTimestamp, interpolatedBearing, interpolatedSpeed);
            break;
        }
        case Data::CLOUD_COVERAGE: {
            CloudCoverageInterpolationValue* leftValue = static_cast<CloudCoverageInterpolationValue*>(left);
            CloudCoverageInterpolationValue* rightValue = static_cast<CloudCoverageInterpolationValue*>(right);
            float interpolatedCloudCoverage = factorB * leftValue->getCloudCoverage() + factorA * rightValue->getCloudCoverage();

            return new CloudCoverageInterpolationValue(priority, interpolatedTimestamp, interpolatedCloudCoverage);
            break;
        }
        default: {
            this->fail("The data type of this filter seems to be invalid.");
            return nullptr;
            break;
        }
    }
}

bool TemporalInterpolationFilter::hasPreprocessed() {
    return this->preprocessed;
}

int TemporalInterpolationFilter::RequestUpdateExtent (
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) {
	if (this->error) {
		return 0;
	}

	vtkInformation* inputInformation = inputVector[0]->GetInformationObject(0);

	// Make the pipeline executive iterate the upstream pipeline time steps by setting the update time step appropiately
	double* inputTimeSteps = inputInformation->Get(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
	if (inputTimeSteps) {
		inputInformation->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), this->currentTimeStep);
	}

	return 1;
}
