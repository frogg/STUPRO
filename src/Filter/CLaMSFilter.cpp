#include <Filter/CLaMSFilter.h>

#include <vtkPolyData.h>
#include <vtkDataObject.h>
#include <vtkAlgorithm.h>
#include <vtkCellArray.h>
#include <vtkObjectFactory.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>


vtkStandardNewMacro(CLaMSFilter);

CLaMSFilter::CLaMSFilter() { };
//CLaMSFilter::CLaMSFilter() : error(false) { }

CLaMSFilter::~CLaMSFilter() { };


void CLaMSFilter::fail(QString message) {
	vtkErrorMacro( << message.toStdString());
	this->error = true;
}



int CLaMSFilter::RequestData(vtkInformation* info,
                             vtkInformationVector** inputVector,
                             vtkInformationVector* outputVector) {
	if (this->error) {
		return 0;
	}

	// Get input and output data from the information vectors
	vtkInformation* inputInformation = inputVector[0]->GetInformationObject(0);
	vtkPolyData* inputData = vtkPolyData::SafeDownCast(inputInformation->Get(
	                             vtkDataObject::DATA_OBJECT()));

	vtkInformation* outputInformation = outputVector->GetInformationObject(0);
	vtkPolyData* output = vtkPolyData::SafeDownCast(outputInformation->Get(
	                          vtkDataObject::DATA_OBJECT()));



	// Create a list of the indices of all points that should be kept by evaluating each one
	QList<int> visiblePoints;
	//interates over all points
	for (int i = 0; i < inputData->GetNumberOfPoints(); i++) {
		double coordinates[3];
		inputData->GetPoint(i, coordinates);
		if (this->evaluatePoint(i, Coordinate(coordinates[0], coordinates[1]), inputData->GetPointData())) {
			visiblePoints.append(i);
		}
	}


	// Create the content of the output poly data object
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
	vertices->Allocate(vertices->EstimateSize(1, visiblePoints.size()));
	vertices->InsertNextCell(visiblePoints.size());

	// Create all arrays from the input data
	QList<vtkSmartPointer<vtkAbstractArray>> inputArrays;
	QList<vtkSmartPointer<vtkAbstractArray>> outputArrays;

	for (int i = 0; i < inputData->GetPointData()->GetNumberOfArrays(); i++) {
		vtkSmartPointer<vtkAbstractArray> inputArray = inputData->GetPointData()->GetAbstractArray(i);

		if (!inputArray) {
			this->fail("An input array could not be read.");
			return 0;
		}

		vtkSmartPointer<vtkAbstractArray> outputArray = vtkAbstractArray::CreateArray(
		            inputArray->GetDataType());

		outputArray->SetNumberOfComponents(inputArray->GetNumberOfComponents());
		outputArray->SetNumberOfTuples(visiblePoints.size());
		outputArray->SetName(inputArray->GetName());

		inputArrays.append(inputArray);
		outputArrays.append(outputArray);
	}

	// Fill the output poly data object with the coordinates of all selected points
	QList<int>::iterator i;
	int tupleNumber = 0;
	for (i = visiblePoints.begin(); i != visiblePoints.end(); ++i) {
		double coordinates[3];
		inputData->GetPoint(*i, coordinates);
		vertices->InsertCellPoint(points->InsertNextPoint(coordinates[0], coordinates[1], coordinates[2]));

		// Copy over all scalars
		for (int j = 0; j < inputArrays.size(); j++) {
			outputArrays.at(j)->SetTuple(tupleNumber, *i, inputArrays.at(j));
		}

		tupleNumber++;
	}

	// Assign the created point set to the output object
	output->SetPoints(points);
	output->SetVerts(vertices);

	// Add the output arrays to the data set
	QList<vtkSmartPointer<vtkAbstractArray>>::iterator j;
	for (j = outputArrays.begin(); j != outputArrays.end(); ++j) {
		output->GetPointData()->AddArray(*j);
	}

	return 1;
}


int CLaMSFilter::RequestInformation(vtkInformation* request,
                                    vtkInformationVector** inputVector,
                                    vtkInformationVector* outputVector) {
	return 1;
}

void CLaMSFilter::setLowerAltitude(float lowerLimit) {
	this->LowerLimitAltitude = lowerLimit;
	this->Modified();
}

void CLaMSFilter::setUpperAltitude(float upperLimit) {
	this->UpperLimitAltitude = upperLimit;
	this->Modified();
}


void CLaMSFilter::setLowerPotTemperature(float lowerLimit) {
	this->lowerPotTemperature = lowerLimit;
	this->Modified();
}

void CLaMSFilter::setUpperPotTemperature(float upperLimit) {
	this->upperPotTemperature = upperLimit;
	this->Modified();
}

void CLaMSFilter::setLowerVorticity(float lowerLimit) {
	this->lowerVorticity = lowerLimit;
	this->Modified();
}

void CLaMSFilter::setUpperVorticity(float upperLimit) {
	this->upperVorticity = upperLimit;
	this->Modified();
}

void CLaMSFilter::setLowerPressure(float lowerLimit) {
	this->lowerPressure = lowerLimit;
	this->Modified();
}

void CLaMSFilter::setUpperPressure(float upperLimit) {
	this->upperPressure = upperLimit;
	this->Modified();
}

void CLaMSFilter::setLowerTemperature(float lowerLimit) {
	this->lowerTemperature = lowerLimit;
	this->Modified();
}

void CLaMSFilter::setUpperTemperature(float upperLimit) {
	this->upperTemperature = upperLimit;
	this->Modified();
}


void CLaMSFilter::setLowerDate(double lowerLimit) {
	this->lowerDate = lowerLimit;
	this->Modified();
}

void CLaMSFilter::setUpperDate(double upperLimit) {
	this->upperDate = upperLimit;
	this->Modified();
}




void CLaMSFilter::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
	os << indent << "Filter for selecting and extracting certain data points, Kronos Project" <<
	   endl;
}
void CLaMSFilter::SetInputConnection(vtkAlgorithmOutput* input) {
	this->Superclass::SetInputConnection(input);
}

int CLaMSFilter::FillOutputPortInformation(int port, vtkInformation* info) {
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
	return 1;
}

int CLaMSFilter::FillInputPortInformation(int port, vtkInformation* info) {
	if (port == 0) {
		info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
		info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
	}

	return 1;
}
bool CLaMSFilter::evaluatePoint(int pointIndex, Coordinate coordinate,
                                vtkPointData* pointData) {
	vtkSmartPointer<vtkFloatArray> altitudeArray = vtkFloatArray::SafeDownCast(
	            pointData->GetAbstractArray("altitude"));
	vtkSmartPointer<vtkFloatArray> PotTemperatureArray = vtkFloatArray::SafeDownCast(
	            pointData->GetAbstractArray("pot_temperature"));
	vtkSmartPointer<vtkFloatArray> vorticityArray = vtkFloatArray::SafeDownCast(
	            pointData->GetAbstractArray("pot_vorticity"));
	vtkSmartPointer<vtkFloatArray> pressureArray = vtkFloatArray::SafeDownCast(
	            pointData->GetAbstractArray("pressure"));
	vtkSmartPointer<vtkFloatArray> temperatureArray = vtkFloatArray::SafeDownCast(
	            pointData->GetAbstractArray("temperature"));
	vtkSmartPointer<vtkDoubleArray> dateArray = vtkDoubleArray::SafeDownCast(
	            pointData->GetAbstractArray("time"));

	float altitude = altitudeArray->GetTuple1(pointIndex);
	float PotTemperature = PotTemperatureArray->GetTuple1(pointIndex);
	float vorticity = vorticityArray->GetTuple1(pointIndex);
	float pressure = pressureArray->GetTuple1(pointIndex);
	float temperature = temperatureArray->GetTuple1(pointIndex);
	double date = dateArray->GetTuple1(pointIndex);

	return (this->LowerLimitAltitude <= altitude && altitude <= this->UpperLimitAltitude)
	       && (this->lowerPressure <= pressure && pressure <= this->upperPressure)
	       && (this->lowerPotTemperature <= PotTemperature && PotTemperature <= this->upperPotTemperature)
	       && (this->lowerVorticity <= vorticity && vorticity <= this-> upperVorticity)
	       && (this-> lowerTemperature <= temperature && temperature <= upperTemperature)
	       && (this-> lowerDate <= date && date <= upperDate);
}
