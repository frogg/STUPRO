#include <Filter/AbstractSelectionFilter.hpp>

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
	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
	
	// Create a human-readable string of all supported data types
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
        supportedTypes.append(" and ").append(Data::getDataTypeName(this->getCompatibleDataTypes().value(amountOfSupportedDataTypes - 1)));
	}

	// Check the meta information containing the data's type
	if (inInfo->Has(Data::VTK_DATA_TYPE())) {
		Data::Type dataType = static_cast<Data::Type>(inInfo->Get(Data::VTK_DATA_TYPE()));
		if (this->getCompatibleDataTypes().contains(dataType)) {
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
