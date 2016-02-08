#include <Filter/PrecipitationTypeFilter.h>

#include <Reader/DataReader/Data.hpp>

#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkIntArray.h>
#include <vtkCompositeDataSet.h>
#include <vtkCompositeDataIterator.h>
#include <vtkHierarchicalBoxDataIterator.h>
#include <vtkTable.h>
#include <vtkGraph.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPointSet.h>

PrecipitationTypeFilter::PrecipitationTypeFilter() {
	// Initialize the selection
	this->selection = vtkSmartPointer<vtkSelection>::New();
	vtkSmartPointer<vtkSelectionNode> selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
	this->selection->AddNode(selectionNode);

	// Make the only selection node filter points by value
	selectionNode->Initialize();
	selectionNode->GetProperties()->Set(vtkSelectionNode::CONTENT_TYPE(), vtkSelectionNode::VALUES);
	selectionNode->GetProperties()->Set(vtkSelectionNode::FIELD_TYPE(), vtkSelectionNode::POINT);

	// Create a list that will store all precipitation types that should be kept
	vtkSmartPointer<vtkIntArray> precipitationTypes = vtkSmartPointer<vtkIntArray>::New();
	precipitationTypes->SetName("precipitationTypes");
	precipitationTypes->SetNumberOfComponents(1);
	precipitationTypes->SetNumberOfTuples(5);

	// Display all precipitation types by default
	for (int i = PrecipitationDataPoint::NONE; i <= PrecipitationDataPoint::HAIL; i++) {
		precipitationTypes->SetTuple1(i - 1, i);
		this->precipitationTypeVisibilities.insert(static_cast<PrecipitationDataPoint::PrecipitationType>
		        (i), true);
	}

	selectionNode->SetSelectionList(precipitationTypes);
}

PrecipitationTypeFilter::~PrecipitationTypeFilter() { }

void PrecipitationTypeFilter::displayPrecipitationType(PrecipitationDataPoint::PrecipitationType
        type, bool display) {
	if (this->error) {
		return;
	}
			
	this->precipitationTypeVisibilities[type] = display;

	// Create the updated selection list this filter uses
	vtkSmartPointer<vtkIntArray> precipitationTypes = vtkSmartPointer<vtkIntArray>::New();
	precipitationTypes->SetName("precipitationTypes");
	precipitationTypes->SetNumberOfComponents(1);
	precipitationTypes->SetNumberOfTuples(5);

	// Refresh the selection list with the updated visibility values
	for (int i = PrecipitationDataPoint::NONE; i <= PrecipitationDataPoint::HAIL; i++) {
		if (this->precipitationTypeVisibilities[static_cast<PrecipitationDataPoint::PrecipitationType>
		                                        (i)]) {
			precipitationTypes->SetTuple1(i - 1, i);
		}
	}

	this->selection->GetNode(0)->SetSelectionList(precipitationTypes);

	this->Modified();
}

void PrecipitationTypeFilter::enableUndefined(int enabled) {
	this->displayPrecipitationType(PrecipitationDataPoint::NONE, enabled);
}

void PrecipitationTypeFilter::enableRain(int enabled) {
	this->displayPrecipitationType(PrecipitationDataPoint::RAIN, enabled);
}

void PrecipitationTypeFilter::enableSnow(int enabled) {
	this->displayPrecipitationType(PrecipitationDataPoint::SNOW, enabled);
}

void PrecipitationTypeFilter::enableSleet(int enabled) {
	this->displayPrecipitationType(PrecipitationDataPoint::SLEET, enabled);
}

void PrecipitationTypeFilter::enableHail(int enabled) {
	this->displayPrecipitationType(PrecipitationDataPoint::HAIL, enabled);
}

void PrecipitationTypeFilter::fail(QString message) {
    vtkErrorMacro(<< message.toStdString());
    this->error = true;
}

vtkStandardNewMacro(PrecipitationTypeFilter);

int PrecipitationTypeFilter::RequestData(vtkInformation* info,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector) {
	if (this->error) {
		return 0;
	}
			
	// Get output information from the request vectors
	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	// Get the actual objects from the obtained information
	vtkDataObject* input = vtkDataObject::GetData(inputVector[0]);
	vtkDataObject* output = vtkDataObject::GetData(outputVector);

	// Check the input for compatibility
	if (!input->IsA("vtkPointSet")) {
		this->fail("The input should be a vtkPointSet.");
		return 0;
	}

	// Actually perform the extraction
	vtkSmartPointer<vtkDataObject> extractedOutput = this->RequestDataFromBlock(input,
	        this->selection->GetNode(0), outInfo);
	if (extractedOutput) {
		output->ShallowCopy(extractedOutput);
	}

	return 1;
}

int PrecipitationTypeFilter::RequestInformation(vtkInformation *request,
				vtkInformationVector **inputVector,
				vtkInformationVector *outputVector) {
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

	if (inInfo->Has(Data::VTK_DATA_TYPE())) {
		Data::Type dataType = static_cast<Data::Type>(inInfo->Get(Data::VTK_DATA_TYPE()));
		if (dataType != Data::PRECIPITATION) {
			this->fail(QString("This filter only works with Precipitation data, but the input contains %1 data.").arg(Data::getDataTypeName(dataType)));
			return 0;
		}
	} else {
		this->fail("This filter only works with data read by the Kronos reader.");
		return 0;
	}

	return 1;
}

void PrecipitationTypeFilter::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
	os << indent << "Precipitation data point extraction based on precipitation type, Kronos Project" <<
	   endl;
}

int PrecipitationTypeFilter::FillOutputPortInformation(int port, vtkInformation* info) {
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPointSet");
	return 1;
}

int PrecipitationTypeFilter::FillInputPortInformation(int port, vtkInformation* info) {
	if (port == 0) {
		info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
		info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
	} else {
		info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
	}

	return 1;
}
