#include <Filter/PrecipitationTypeFilter.h>

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

PrecipitationTypeFilter::PrecipitationTypeFilter() {
	// Initialize the selection
	this->selection = vtkSmartPointer<vtkSelection>::New();
	this->selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
	this->selection->AddNode(selectionNode);

	// Make the only selection node filter points by value
	this->selectionNode->Initialize();
	this->selectionNode->GetProperties()->Set(vtkSelectionNode::CONTENT_TYPE(),
	        vtkSelectionNode::VALUES);
	this->selectionNode->GetProperties()->Set(vtkSelectionNode::FIELD_TYPE(), vtkSelectionNode::POINT);

	// Create a list that will store all precipitation types that should be kept
	vtkSmartPointer<vtkIntArray> precipitationTypes = vtkSmartPointer<vtkIntArray>::New();
	precipitationTypes->SetName("precipitationTypes");
	precipitationTypes->SetNumberOfComponents(1);
	precipitationTypes->SetNumberOfTuples(5);

	// Display all precipitation types by default
	for (int i = PrecipitationDataPoint::NONE; i <= PrecipitationDataPoint::HAIL; i++) {
		precipitationTypes->SetTuple1(i - 1, i);
	}

	// Create the map that will store the precipitation type's visibilities
	this->precipitationTypeVisibilities.insert(PrecipitationDataPoint::NONE, true);
	this->precipitationTypeVisibilities.insert(PrecipitationDataPoint::RAIN, true);
	this->precipitationTypeVisibilities.insert(PrecipitationDataPoint::SNOW, true);
	this->precipitationTypeVisibilities.insert(PrecipitationDataPoint::SLEET, true);
	this->precipitationTypeVisibilities.insert(PrecipitationDataPoint::HAIL, true);

	this->selectionNode->SetSelectionList(precipitationTypes);
}

PrecipitationTypeFilter::~PrecipitationTypeFilter() { }

void PrecipitationTypeFilter::displayPrecipitationType(PrecipitationDataPoint::PrecipitationType
        type, bool display) {
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

	this->selectionNode->SetSelectionList(precipitationTypes);

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

vtkStandardNewMacro(PrecipitationTypeFilter);

int PrecipitationTypeFilter::RequestData(vtkInformation* info,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector) {
	// Get input and output information from the request vectors
	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	// Get the actual objects from the obtained information
	vtkDataObject* input = vtkDataObject::GetData(inInfo);
	vtkDataObject* output = vtkDataObject::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	// Check the input for compatibility
	if (!input) {
		vtkErrorMacro( << "No input specified.");
		return 0;
	}

	if (vtkGraph::SafeDownCast(input)) {
		vtkErrorMacro( << "The input should not be a graph.");
		return 0;
	}

	// Actually perform the extraction
	if (input->IsA("vtkCompositeDataSet")) {
		vtkErrorMacro( << "The input should not be a composite data set.");
		return 0;
	} else {
		vtkSmartPointer<vtkSelectionNode> node = vtkSmartPointer<vtkSelectionNode>::New();
		if (this->selection->GetNumberOfNodes() > 0) {
			node = this->selection->GetNode(0);
		}

		vtkSmartPointer<vtkDataObject> extractedOutput = this->RequestDataFromBlock(input, node, outInfo);
		if (extractedOutput) {
			output->ShallowCopy(extractedOutput);
		}
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
