#include <Filter/TemporalAggregationFilter.h>

#include <Filter/TemporalAggregationFilter/PrecipitationAggregationValue.hpp>
#include <Filter/TemporalAggregationFilter/TemperatureAggregationValue.hpp>
#include <Filter/TemporalAggregationFilter/WindAggregationValue.hpp>
#include <Filter/TemporalAggregationFilter/CloudCoverageAggregationValue.hpp>

#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkDataArray.h>
#include <vtkTypeFloat32Array.h>

vtkStandardNewMacro(TemporalAggregationFilter);

const QList<Data::Type> TemporalAggregationFilter::SUPPORTED_DATA_TYPES = QList<Data::Type>() << Data::PRECIPITATION << Data::TEMPERATURE << Data::WIND << Data::CLOUD_COVERAGE;

TemporalAggregationFilter::TemporalAggregationFilter() : error(false), currentTimeStep(0) {
    this->SetNumberOfInputPorts(1);
    this->SetNumberOfOutputPorts(1);
}

TemporalAggregationFilter::~TemporalAggregationFilter() {
    qDeleteAll(this->aggregatedData);
    this->aggregatedData.clear();
}

void TemporalAggregationFilter::fail(QString message) {
	vtkErrorMacro( << message.toStdString());
	this->error = true;
}

void TemporalAggregationFilter::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);

    os << indent << "Temporal data point aggregation, Kronos Project" << endl;
}

int TemporalAggregationFilter::FillInputPortInformation(int port, vtkInformation* info) {
    if (port == 0) {
		info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
		info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
	}
    
    return 1;
}

int TemporalAggregationFilter::FillOutputPortInformation(int port, vtkInformation* info) {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
    return 1;
}

int TemporalAggregationFilter::RequestInformation (
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector) {
    if (this->error) {
        return 0;
    }

    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    
    if (inInfo->Has(Data::VTK_DATA_TYPE())) {
		this->dataType = static_cast<Data::Type>(inInfo->Get(Data::VTK_DATA_TYPE()));
	} else {
		this->fail("This filter only works with data read by the Kronos reader.");
		return 0;
	}
    
    // Overkill code for creating a comma-separated string with the names of all supported data types
    QString supportedTypes = "";
    int amountOfSupportedDataTypes = TemporalAggregationFilter::SUPPORTED_DATA_TYPES.size();
    if (amountOfSupportedDataTypes == 1) {
        supportedTypes.append(Data::getDataTypeName(TemporalAggregationFilter::SUPPORTED_DATA_TYPES.value(0)));
    } else if (amountOfSupportedDataTypes > 1) {
        for (int i = 0; i < amountOfSupportedDataTypes - 2; i++) {
            supportedTypes.append(Data::getDataTypeName(TemporalAggregationFilter::SUPPORTED_DATA_TYPES.value(i)));
            if (i < amountOfSupportedDataTypes - 3) {
                supportedTypes.append(", ");
            }
        }
        supportedTypes.append(" and ").append(Data::getDataTypeName(TemporalAggregationFilter::SUPPORTED_DATA_TYPES.value(amountOfSupportedDataTypes - 1)));
    }
    
    if (!TemporalAggregationFilter::SUPPORTED_DATA_TYPES.contains(this->dataType)) {
        this->fail(QString("This filter only supports %1 data, but the input contains %2 data.").arg(supportedTypes, Data::getDataTypeName(this->dataType)));
        return 0;
    }
    
    // This filter's output is an aggregation of values over time and therefore has no time information
    outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
    outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_RANGE());

    return 1;
}

int TemporalAggregationFilter::RequestData(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector) {
    if (this->error) {
        return 0;
    }
    
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    // vtkInformation *outInfo = outputVector->GetInformationObject(0);

    vtkPolyData *input = vtkPolyData::GetData(inInfo);
    // vtkPolyData *output = vtkPolyData::GetData(outInfo);
    
    std::cout << "Current timestep: " << this->currentTimeStep << std::endl;
    
    // Add the data from the current timestep to the accumulation
    for (int i = 0; i < input->GetNumberOfPoints(); i++) {
        double coordinates[3];
        input->GetPoint(i, coordinates);
        PointCoordinates currentCoordinates(coordinates[0], coordinates[1], coordinates[2]);
        
        switch (this->dataType) {
            case Data::PRECIPITATION: {
                break; }
            case Data::TEMPERATURE: {
                vtkSmartPointer<vtkDataArray> abstractTemperatureArray = input->GetPointData()->GetArray("temperatures");
            	vtkSmartPointer<vtkTypeFloat32Array> temperatureArray = vtkTypeFloat32Array::SafeDownCast(abstractTemperatureArray);
                double currentTemperature = temperatureArray->GetValue(i);
            
                if (this->aggregatedData.contains(currentCoordinates)) {
                    // The point has already been looked at before
                    TemperatureAggregationValue* currentValue = static_cast<TemperatureAggregationValue*>(this->aggregatedData.value(currentCoordinates));
                
                    // Calculate the arithmetric mean with the cumulative moving average method.
                    // This is a bit more costly than the naive way to calculate the average but prevents huge numbers from showing up while summing up all values.
                    currentValue->setAverageTemperature((currentTemperature + (currentValue->getTimeIndex() * currentValue->getAverageTemperature())) / (currentValue->getTimeIndex() * 1.0 + 1));
                    currentValue->setTimeIndex(currentValue->getTimeIndex() + 1);
                } else {
                    // This is the first time a point with these coordinates shows up
                    TemperatureAggregationValue* newValue = new TemperatureAggregationValue();
                    newValue->setAverageTemperature(currentTemperature);
                    this->aggregatedData.insert(currentCoordinates, newValue);
                }
                break; }
            case Data::WIND: {
                vtkSmartPointer<vtkDataArray> abstractVelocitiesArray = input->GetPointData()->GetArray("speeds");
            	vtkSmartPointer<vtkTypeFloat32Array> velocitiesArray = vtkTypeFloat32Array::SafeDownCast(abstractVelocitiesArray);
                double currentVelocity = velocitiesArray->GetValue(i);
                
                vtkSmartPointer<vtkDataArray> abstractBearingsArray = input->GetPointData()->GetArray("directions");
                vtkSmartPointer<vtkTypeFloat32Array> bearingsArray = vtkTypeFloat32Array::SafeDownCast(abstractBearingsArray);
                double currentBearing = bearingsArray->GetValue(i);
            
                if (this->aggregatedData.contains(currentCoordinates)) {
                    // The point has already been looked at before
                    WindAggregationValue* currentValue = static_cast<WindAggregationValue*>(this->aggregatedData.value(currentCoordinates));
                
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
                    this->aggregatedData.insert(currentCoordinates, newValue);
                }
                break; }
            case Data::CLOUD_COVERAGE: {
                vtkSmartPointer<vtkDataArray> abstractCoverageArray = input->GetPointData()->GetArray("cloudCovers");
            	vtkSmartPointer<vtkTypeFloat32Array> coverageArray = vtkTypeFloat32Array::SafeDownCast(abstractCoverageArray);
                double currentCoverage = coverageArray->GetValue(i);
            
                if (this->aggregatedData.contains(currentCoordinates)) {
                    // The point has already been looked at before
                    CloudCoverageAggregationValue* currentValue = static_cast<CloudCoverageAggregationValue*>(this->aggregatedData.value(currentCoordinates));
                
                    // Calculate the arithmetric mean with the cumulative moving average method.
                    // This is a bit more costly than the naive way to calculate the average but prevents huge numbers from showing up while summing up all values.
                    currentValue->setAverageCloudCoverage((currentCoverage + (currentValue->getTimeIndex() * currentValue->getAverageCloudCoverage())) / (currentValue->getTimeIndex() * 1.0 + 1));
                    currentValue->setTimeIndex(currentValue->getTimeIndex() + 1);
                } else {
                    // This is the first time a point with these coordinates shows up
                    CloudCoverageAggregationValue* newValue = new CloudCoverageAggregationValue();
                    newValue->setAverageCloudCoverage(currentCoverage);
                    this->aggregatedData.insert(currentCoordinates, newValue);
                }
                break; }
            default:
                break;
        }
    }

    this->currentTimeStep++;

    if (this->currentTimeStep < inInfo->Length(vtkStreamingDemandDrivenPipeline::TIME_STEPS())) {
        // There are still time steps left, continue on
        request->Set(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING(), 1);
    } else {
        // Everything has been accumulated
        // TODO: Finish up and output data
        
        request->Remove(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING());
        this->currentTimeStep = 0;
    }
    
    return 1;
}

int TemporalAggregationFilter::RequestUpdateExtent (
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector) {
    if (this->error) {
        return 0;
    }
    
    // TODO: Update progress
    
    vtkInformation *inputInformation = inputVector[0]->GetInformationObject(0);

    // Make the pipeline executive iterate the upstream pipeline time steps by setting the update time step appropiately
    double *inputTimeSteps = inputInformation->Get(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
    if (inputTimeSteps) {
        inputInformation->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), inputTimeSteps[this->currentTimeStep]);
    }

    return 1;
}