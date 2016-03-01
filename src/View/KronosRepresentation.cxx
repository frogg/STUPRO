#include <Reader/DataReader/Data.hpp>

#include "KronosRepresentation.h"
#include "vtkObjectFactory.h"
#include <vtkPointSetToLabelHierarchy.h>
#include <vtkLabelPlacementMapper.h>
#include "vtkPVRenderView.h"
#include "vtkRenderer.h"
#include "vtkStringArray.h"
#include "vtkPointSetToLabelHierarchy.h"
#include "vtkPointData.h"
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkIntArray.h>
vtkStandardNewMacro(KronosRepresentation);
//----------------------------------------------------------------------------
KronosRepresentation::KronosRepresentation() {
	this->error = false;
	//Set nummber of input connections.
	this->SetNumberOfInputPorts(1);
	this->SetNumberOfOutputPorts(0);
	//Create Object of Everything used;
	this->pointActor = vtkSmartPointer<vtkActor>::New();
	this->labelActor = vtkSmartPointer<vtkActor2D>::New();
	this->pointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	this->labelMapper = vtkSmartPointer<vtkLabelPlacementMapper>::New();
	this->pointSetToLabelHierarchyFilter = vtkSmartPointer<vtkPointSetToLabelHierarchy>::New();
	//Set visibility to false so its not shown on load
	this->SetVisibility(false);
	this->useDepthBuffer = true;
}

//----------------------------------------------------------------------------
KronosRepresentation::~KronosRepresentation() {}
//---------------------------------------------------------------------------
int KronosRepresentation::RequestInformation(vtkInformation* request,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector) {
	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);

	if (inInfo->Has(Data::VTK_DATA_TYPE())) {
		Data::Type dataType = static_cast<Data::Type>(inInfo->Get(Data::VTK_DATA_TYPE()));
		if (dataType != Data::CITIES) {
			this->fail(
			    QString("This filter only works with Cities data, but the input contains %1 data.").arg(
			        Data::getDataTypeName(dataType)));
			return 0;
		}
	} else {
		this->fail("This filter only works with data read by the Kronos reader.");
		return 0;
	}

	return 1;
}
//----------------------------------------------------------------------------
int KronosRepresentation::RequestData(vtkInformation* request,
                                      vtkInformationVector** inputVector, vtkInformationVector* outputVector) {
	//Quit if an error occured.
	if (this->error) {
		return 0;
	}
	if (inputVector[0]->GetNumberOfInformationObjects() == 1) {
		vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
		vtkPolyData* input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));

		//Mapper for the points
		this->pointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		this->pointMapper->SetInputData(input);

		//Generate the label hierarchy and filter the points according to the priorities.
		this->pointSetToLabelHierarchyFilter = vtkSmartPointer<vtkPointSetToLabelHierarchy>::New();
		this->pointSetToLabelHierarchyFilter->SetInputData(input);
		this->pointSetToLabelHierarchyFilter->SetLabelArrayName("names");
		this->pointSetToLabelHierarchyFilter->SetPriorityArrayName("priorities");
		this->pointSetToLabelHierarchyFilter->Update();

		//Create the labelmapper
		this->labelMapper = vtkSmartPointer<vtkLabelPlacementMapper>::New();
		this->labelMapper->SetInputConnection(pointSetToLabelHierarchyFilter->GetOutputPort());
		//Use depth buffer
		if (this->useDepthBuffer) {
			this->labelMapper->UseDepthBufferOn();
		} else {
			this->labelMapper->UseDepthBufferOff();
		}
		//Add the mappers to the actors.
		this->pointActor->SetMapper(this->pointMapper);
		this->labelActor->SetMapper(labelMapper);

		//In order for the depthbuffer to work we need to delete the input now.
		inputVector[0]->Remove(inInfo);
	}
	//Call superclass to prevent warnings.
	return this->Superclass::RequestData(request, inputVector, outputVector);

}

//----------------------------------------------------------------------------
void KronosRepresentation::SetVisibility(bool val) {
	//Set visibility of the actors and superclass.
	this->Superclass::SetVisibility(val);
	this->labelActor->SetVisibility(val ?  1 : 0);
	this->pointActor->SetVisibility(val ?  1 : 0);
}
void KronosRepresentation::SetDepthBuffer(bool val) {
	this->useDepthBuffer = val;
}

//----------------------------------------------------------------------------
bool KronosRepresentation::AddToView(vtkView* view) {
	//Adds the actors to the view.
	vtkPVRenderView* rview = vtkPVRenderView::SafeDownCast(view);
	if (rview) {
		rview->GetRenderer()->AddActor(this->pointActor);
		rview->GetRenderer()->AddActor(this->labelActor);
	}
	return this->Superclass::AddToView(view);
}
bool KronosRepresentation::RemoveFromView(vtkView* view) {
	//Removes the actors from the view.
	vtkPVRenderView* rview = vtkPVRenderView::SafeDownCast(view);
	if (rview) {
		rview->GetRenderer()->RemoveActor(this->pointActor);
		rview->GetRenderer()->RemoveActor(this->labelActor);
	}
	return this->Superclass::RemoveFromView(view);
}
//----------------------------------------------------------------------------
int KronosRepresentation::FillInputPortInformation(int, vtkInformation* info) {
	//The input data needs to be polydata.
	info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
	return 1;
}
//----------------------------------------------------------------------------
void KronosRepresentation::fail(QString message) {
	vtkErrorMacro( << message.toStdString());
	this->error = true;
}
//----------------------------------------------------------------------------
void KronosRepresentation::PrintSelf(ostream& os, vtkIndent indent) {
	//Print superclass
	this->Superclass::PrintSelf(os, indent);
	os << indent <<
	   "Representation for Data read by the Kronosreader. This class is part of the Kronos Project." <<
	   endl;
}
