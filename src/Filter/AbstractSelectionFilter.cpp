#include <Filter/AbstractSelectionFilter.hpp>

#include <vtkPolyData.h>
#include <vtkDataObject.h>
#include <vtkAlgorithm.h>
#include <vtkCellArray.h>

AbstractSelectionFilter::AbstractSelectionFilter() : error(false) { }

AbstractSelectionFilter::~AbstractSelectionFilter() { }

void AbstractSelectionFilter::fail(QString message) {
	vtkErrorMacro( << message.toStdString());
	this->error = true;
}

int AbstractSelectionFilter::RequestData(vtkInformation* info,
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
	QList<int> selectedPoints;

	for (int i = 0; i < inputData->GetNumberOfPoints(); i++) {
		double coordinates[3];
		inputData->GetPoint(i, coordinates);
		if (this->evaluatePoint(i, Coordinate(coordinates[0], coordinates[1]), inputData->GetPointData())) {
			selectedPoints.append(i);
		}
	}

	// Create the content of the output poly data object
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
	vertices->Allocate(vertices->EstimateSize(1, selectedPoints.size()));
	vertices->InsertNextCell(selectedPoints.size());

	// Create all arrays from the input data
	QList<vtkSmartPointer<vtkAbstractArray>> inputArrays;
	QList<vtkSmartPointer<vtkAbstractArray>> outputArrays;

	for (int i = 0; i < inputData->GetPointData()->GetNumberOfArrays(); i++) {
		vtkSmartPointer<vtkAbstractArray> inputArray = inputData->GetPointData()->GetArray(i);
		vtkSmartPointer<vtkAbstractArray> outputArray = vtkAbstractArray::CreateArray(
		            inputArray->GetDataType());

		outputArray->SetNumberOfComponents(inputArray->GetNumberOfComponents());
		outputArray->SetNumberOfTuples(selectedPoints.size());
		outputArray->SetName(inputArray->GetName());

		inputArrays.append(inputArray);
		outputArrays.append(outputArray);
	}

	// Fill the output poly data object with the coordinates of all selected points
	QList<int>::iterator i;
	int tupleNumber = 0;
	for (i = selectedPoints.begin(); i != selectedPoints.end(); ++i) {
		double coordinates[3];
		inputData->GetPoint(*i, coordinates);

		vertices->InsertCellPoint(points->InsertNextPoint(coordinates[0], coordinates[1], 0));

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

int AbstractSelectionFilter::RequestInformation(vtkInformation* request,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector) {
	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);

	// Create a human-readable string of all supported data types for potentially showing an error message
	QString supportedTypes = "";
	int amountOfSupportedDataTypes = this->getCompatibleDataTypes().size();
	if (amountOfSupportedDataTypes == 1) {
		supportedTypes.append(Data::getDataTypeName(this->getCompatibleDataTypes().value(0)));
	} else if (amountOfSupportedDataTypes > 1) {
		for (int i = 0; i < amountOfSupportedDataTypes - 2; i++) {
			supportedTypes.append(Data::getDataTypeName(this->getCompatibleDataTypes().value(i)));
			if (i < amountOfSupportedDataTypes - 3) {
				supportedTypes.append(", ");
			}
		}
		supportedTypes.append(" and ").append(Data::getDataTypeName(this->getCompatibleDataTypes().value(
		        amountOfSupportedDataTypes - 1)));
	}

	// Check the meta information containing the data's type
	if (inInfo->Has(Data::VTK_DATA_TYPE())) {
		Data::Type dataType = static_cast<Data::Type>(inInfo->Get(Data::VTK_DATA_TYPE()));
		if (!this->getCompatibleDataTypes().contains(dataType)) {
			this->fail(
			    QString("This filter only works with %1 data, but the input contains %2 data.").arg(supportedTypes,
			            Data::getDataTypeName(dataType)));
			return 0;
		}
	} else {
		this->fail("This filter only works with data read by the Kronos reader.");
		return 0;
	}

	return 1;
}

void AbstractSelectionFilter::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
	os << indent << "Filter for selecting and extracting certain data points, Kronos Project" <<
	   endl;
}

int AbstractSelectionFilter::FillOutputPortInformation(int port, vtkInformation* info) {
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");

	return 1;
}

int AbstractSelectionFilter::FillInputPortInformation(int port, vtkInformation* info) {
	if (port == 0) {
		info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
		info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
	}

	return 1;
}
