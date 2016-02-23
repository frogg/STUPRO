#include <Filter/TemporalAggregationFilter.h>

#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>

vtkStandardNewMacro(TemporalAggregationFilter);

const QList<Data::Type> TemporalAggregationFilter::SUPPORTED_DATA_TYPES = QList<Data::Type>() <<
        Data::PRECIPITATION << Data::TEMPERATURE << Data::WIND << Data::CLOUD_COVERAGE;

TemporalAggregationFilter::TemporalAggregationFilter() : error(false), currentTimeStep(0),
	dataAggregator() {
	this->SetNumberOfInputPorts(1);
	this->SetNumberOfOutputPorts(1);
}

TemporalAggregationFilter::~TemporalAggregationFilter() { }

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
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) {
	if (this->error) {
		return 0;
	}

	vtkInformation* outInfo = outputVector->GetInformationObject(0);
	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);

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
		supportedTypes.append(Data::getDataTypeName(TemporalAggregationFilter::SUPPORTED_DATA_TYPES.value(
		                          0)));
	} else if (amountOfSupportedDataTypes > 1) {
		for (int i = 0; i < amountOfSupportedDataTypes - 2; i++) {
			supportedTypes.append(Data::getDataTypeName(TemporalAggregationFilter::SUPPORTED_DATA_TYPES.value(
			                          i)));
			if (i < amountOfSupportedDataTypes - 3) {
				supportedTypes.append(", ");
			}
		}
		supportedTypes.append(" and ").append(Data::getDataTypeName(
		        TemporalAggregationFilter::SUPPORTED_DATA_TYPES.value(amountOfSupportedDataTypes - 1)));
	}

	if (!TemporalAggregationFilter::SUPPORTED_DATA_TYPES.contains(this->dataType)) {
		this->fail(QString("This filter only supports %1 data, but the input contains %2 data.").arg(
		               supportedTypes, Data::getDataTypeName(this->dataType)));
		return 0;
	}

	// Check the meta information containing the data's state
	if (inInfo->Has(Data::VTK_DATA_STATE())) {
		Data::State dataState = static_cast<Data::State>(inInfo->Get(Data::VTK_DATA_STATE()));
		if (dataState != Data::RAW) {
			this->fail(
			    QString("This filter only works with raw input data, but the input data has the state %1.").arg(
			        Data::getDataStateName(dataState)));
			return 0;
		}
	} else {
		this->fail("The input data has no data state information attached.");
		return 0;
	}

	// Extract the time resolution from the input data's meta data
	if (inInfo->Has(Data::VTK_TIME_RESOLUTION())) {
		this->timeResolution = inInfo->Get(Data::VTK_TIME_RESOLUTION());
	} else {
		if (this->dataType == Data::PRECIPITATION) {
			// Currently, the time resolution is only really critical for the accumulation of precipitation data
			this->fail("To correctly accumulate precipitation data, this filter needs the data's time resolution.");
			return 0;
		}
	}

	// Pass all data set information to the aggregator
	this->dataAggregator.setDataSetAttributes(this->dataType, this->timeResolution);

	// This filter's output is an aggregation of values over time and therefore has no time information
	outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
	outInfo->Remove(vtkStreamingDemandDrivenPipeline::TIME_RANGE());

	// Update the state of the output data
	outInfo->Set(Data::VTK_DATA_STATE(), Data::AGGREGATED);

	return 1;
}

int TemporalAggregationFilter::RequestData(
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

	if (this->currentTimeStep == 0) {
		this->SetProgressText("Aggregating data...");
		this->SetProgress(0.0);
	}

	// Add the data from the current timestep to the accumulation
	for (int i = 0; i < input->GetNumberOfPoints(); i++) {
		double coordinates[3];
		input->GetPoint(i, coordinates);
		PointCoordinates currentCoordinates(coordinates[0], coordinates[1], coordinates[2]);

		this->dataAggregator.addPointData(i, currentCoordinates, this->currentTimeStep,
		                                  input->GetPointData());
	}

	if (this->currentTimeStep < inInfo->Length(vtkStreamingDemandDrivenPipeline::TIME_STEPS())) {
		// There are still time steps left, continue on
		request->Set(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING(), 1);
		this->currentTimeStep++;

		// Update the progress
		double progress = (double) (this->currentTimeStep / (1.0 * inInfo->Length(
		                                vtkStreamingDemandDrivenPipeline::TIME_STEPS())));
		this->UpdateProgress(progress);
	} else {
		// Everything has been accumulated
		output->DeepCopy(this->dataAggregator.getPolyData());
		request->Remove(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING());
		this->currentTimeStep = 0;
		this->dataAggregator.clearAggregationData();
		this->SetProgressText("");
		this->SetProgress(1.0);
	}

	return 1;
}

int TemporalAggregationFilter::RequestUpdateExtent (
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