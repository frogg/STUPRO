#include <Filter/AbstractSelectionFilter.h>

#include <Reader/DataReader/Data.hpp>

AbstractSelectionFilter::AbstractSelectionFilter() : error(false) { }

AbstractSelectionFilter::~AbstractSelectionFilter() { }

void AbstractSelectionFilter::fail(QString message) {
	vtkErrorMacro( << message.toStdString());
	this->error = true;
}

vtkStandardNewMacro(AbstractSelectionFilter);

int AbstractSelectionFilter::RequestData(vtkInformation* info,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector) {
	if (this->error) {
		return 0;
	}
    
    // TODO: Loop through points and extract all valid ones

	return 1;
}

int AbstractSelectionFilter::RequestInformation(vtkInformation* request,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector) {
    
    // TODO: Verify data types

	return 1;
}

void AbstractSelectionFilter::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
	os << indent << "Filter for selecting and extracting certain data points, Kronos Project" <<
	   endl;
}

int AbstractSelectionFilter::FillOutputPortInformation(int port, vtkInformation* info) {
    if (port == 0) {
        info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPointSet");
    }
    
	return 1;
}

int AbstractSelectionFilter::FillInputPortInformation(int port, vtkInformation* info) {
	if (port == 0) {
		info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
		info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
	}

	return 1;
}
