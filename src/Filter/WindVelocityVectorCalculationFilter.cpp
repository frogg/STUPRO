#include <Filter/WindVelocityVectorCalculationFilter.hpp>

#include <vtkFloatArray.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkExecutive.h>
#include <vtkInformationExecutivePortVectorKey.h>

#include <Utils/Math/Functions.hpp>

WindVelocityVectorCalculationFilter::WindVelocityVectorCalculationFilter() : error(false) { }
WindVelocityVectorCalculationFilter::~WindVelocityVectorCalculationFilter() { }

void WindVelocityVectorCalculationFilter::fail(QString message) {
	vtkErrorMacro( << message.toStdString());
	this->error = true;
}

int WindVelocityVectorCalculationFilter::RequestData(vtkInformation* info,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector) {
	if (this->error) {
		return 0;
	}

	// Get input and output data from the information vectors
	vtkInformation* inputInformation = inputVector[0]->GetInformationObject(0);
	vtkPointSet* inputData = vtkPointSet::SafeDownCast(inputInformation->Get(
	                             vtkDataObject::DATA_OBJECT()));

	vtkInformation* outputInformation = outputVector->GetInformationObject(0);
	vtkPolyData* outputData = vtkPolyData::SafeDownCast(outputInformation->Get(
	                          vtkDataObject::DATA_OBJECT()));
	
	// Get the right array depending on this filter's input data state
	QString speedArrayName;
	QString directionArrayName;
	
	if (this->dataState == Data::RAW) {
		speedArrayName = "speeds";
		directionArrayName = "directions";
	} else {
		speedArrayName = "Average Wind Speeds";
		directionArrayName = "Average Wind Directions";
	}
							  
	// Extract data arrays from the input data
	vtkSmartPointer<vtkFloatArray> speedArray = vtkFloatArray::SafeDownCast(inputData->GetPointData()->GetArray(speedArrayName.toStdString().c_str()));
	vtkSmartPointer<vtkFloatArray> directionArray = vtkFloatArray::SafeDownCast(inputData->GetPointData()->GetArray(directionArrayName.toStdString().c_str()));
	
	// Copy over all existing information since we only want to add an array
	outputData->DeepCopy(inputData);

	// Create the velocity array
	vtkSmartPointer<vtkFloatArray> velocities = vtkSmartPointer<vtkFloatArray>::New();
	velocities->SetNumberOfComponents(3);
	velocities->SetNumberOfTuples(inputData->GetNumberOfPoints());
	velocities->SetName("velocity");
	
	int tupleNumber = 0;
	for (int i = 0; i < inputData->GetNumberOfPoints(); i++) {
		// Calculate each point's velocity for use in flow visualisation
		float windDirectionRadian = toRadians(directionArray->GetTuple1(i));
		double windSpeed = speedArray->GetTuple1(i);

		double velocity[3] = {
			(double) windSpeed * sin(windDirectionRadian),
			(double) windSpeed * cos(windDirectionRadian),
			0.0
		};
		velocities->SetTuple(tupleNumber, velocity);

		tupleNumber++;
	}

	outputData->GetPointData()->SetVectors(velocities);

	return 1;
}

int WindVelocityVectorCalculationFilter::RequestInformation(vtkInformation* request,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector) {
	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation* outInfo = outputVector->GetInformationObject(0);
	
	// Check the meta information containing the data's type
	if (inInfo->Has(Data::VTK_DATA_TYPE())) {
		Data::Type dataType = static_cast<Data::Type>(inInfo->Get(Data::VTK_DATA_TYPE()));
		if (dataType != Data::WIND) {
			this->fail(QString("This filter calculates velocity vectors for wind data and therefore only works with wind data, but the input contains %1 data.").arg(Data::getDataTypeName(dataType)));
			return 0;
		}
	} else {
		this->fail("This filter only works with data read by the Kronos reader.");
		return 0;
	}

	// Check the meta information containing the data's state
	if (vtkExecutive::CONSUMERS()->Length(outInfo) == 0) {
		if (inInfo->Has(Data::VTK_DATA_STATE())) {
			this->dataState = static_cast<Data::State>(inInfo->Get(Data::VTK_DATA_STATE()));

			if (this->dataState != Data::RAW && this->dataState != Data::AGGREGATED) {
				this->fail(
				    QString("This filter only works with raw and aggregated input data, but the input data has the state %1.").arg(
				        Data::getDataStateName(this->dataState)));
				return 0;
			}
		} else {
			this->fail("The input data has no data state information attached.");
			return 0;
		}
	}

	return 1;
}

void WindVelocityVectorCalculationFilter::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
	os << indent << "Filter for calculating velocity vectors from wind data, Kronos Project" <<
	   endl;
}

int WindVelocityVectorCalculationFilter::FillOutputPortInformation(int port, vtkInformation* info) {
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");

	return 1;
}

int WindVelocityVectorCalculationFilter::FillInputPortInformation(int port, vtkInformation* info) {
	if (port == 0) {
		info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
		info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
		info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
		info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkUnstructuredGrid");
		info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
	}

	return 1;
}
