#include <Utils/Config/Configuration.hpp>
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
#include <vtkCamera.h>
#include <pqApplicationCore.h>
vtkStandardNewMacro(KronosRepresentation);
//----------------------------------------------------------------------------
KronosRepresentation::KronosRepresentation() : error(false), inPowerwallMode(false) {
	

	//Create Object of Everything used;
	this->labelActor = vtkSmartPointer<vtkActor2D>::New();
	this->pointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	this->labelMapper = vtkSmartPointer<KronosLabelMapper>::New();
	this->pointSetToLabelHierarchyFilter = vtkSmartPointer<vtkPointSetToLabelHierarchy>::New();
	this->inputData =  vtkSmartPointer<vtkPolyData>::New();
	//Set visibility to false so its not shown on load
	this->SetVisibility(false);
	//Get DepthBuffer Option from Config
	QString key = "representation.useCulling";
	if (Configuration::getInstance().hasKey(key)) {
		this->useDepthBuffer = Configuration::getInstance().getBoolean(key);
	} else {
		this->useDepthBuffer = true;
	}

}

//----------------------------------------------------------------------------
KronosRepresentation::~KronosRepresentation() {}

//---------------------------------------------------------------------------
int KronosRepresentation::RequestInformation(vtkInformation* request,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector) {
	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
	if (inInfo == nullptr) {
		this->inPowerwallMode = true;
		return 1;
	}
	if (inInfo->Has(Data::VTK_DATA_TYPE())) {
		Data::Type dataType = static_cast<Data::Type>(inInfo->Get(Data::VTK_DATA_TYPE()));
        if (dataType != Data::CITIES && dataType != Data::TWEETS && dataType != Data::FLIGHTS) {
			this->fail(
			    QString("This Representation only works with City and Tweet data, but the input contains %1 data.").arg(
			        Data::getDataTypeName(dataType)));
			return 0;
		}
		this->currentDataType = dataType;
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

		if (this->inPowerwallMode && false) {
			if (input->GetPointData()->HasArray("names")) {
				currentDataType = Data::CITIES;

			}
			if (input->GetPointData()->HasArray("authors")) {
				currentDataType = Data::TWEETS;
			}
            if (input->GetPointData()->HasArray("airlines")) {
                currentDataType = Data::FLIGHTS;
            }
            
		}
		switch (currentDataType) {
		case Data::CITIES:
            CitiesRepresentation(input);
			break;
		case Data::TWEETS:
			//TweetRepresentation(input);
			break;
        case Data::FLIGHTS:
            //FlightRepresentation(input);
            break;
        default:
            break;
		};

	}

	// Call superclass to draw points.
	return this->Superclass::RequestData(request, inputVector, outputVector);

}
//----------------------------------------------------------------------------
void KronosRepresentation::CitiesRepresentation(vtkPolyData* input) {
	//Generate the label hierarchy and filter the points according to the priorities.
	this->pointSetToLabelHierarchyFilter = vtkSmartPointer<vtkPointSetToLabelHierarchy>::New();
	this->pointSetToLabelHierarchyFilter->SetInputData(input);
	this->pointSetToLabelHierarchyFilter->SetLabelArrayName("names");
	this->pointSetToLabelHierarchyFilter->SetPriorityArrayName("priorities");
	this->pointSetToLabelHierarchyFilter->Update();

	//Create the labelmapper
	this->labelMapper = vtkSmartPointer<KronosLabelMapper>::New();
	this->labelMapper->SetInputConnection(pointSetToLabelHierarchyFilter->GetOutputPort());
	this->labelMapper->SetUseDepthBuffer(this->useDepthBuffer);

	//Add the mappers to the actors.
	this->labelActor->SetMapper(labelMapper);
}
//----------------------------------------------------------------------------
void KronosRepresentation::TweetRepresentation(vtkPolyData* input) {


	//Generate the label hierarchy and filter the points according to the priorities.
	this->pointSetToLabelHierarchyFilter = vtkSmartPointer<vtkPointSetToLabelHierarchy>::New();
	this->pointSetToLabelHierarchyFilter->SetInputData(input);
	this->pointSetToLabelHierarchyFilter->SetLabelArrayName("authors");
	this->pointSetToLabelHierarchyFilter->SetPriorityArrayName("priorities");
	this->pointSetToLabelHierarchyFilter->Update();

	//Create the labelmapper
	this->labelMapper = vtkSmartPointer<KronosLabelMapper>::New();
	this->labelMapper->SetInputConnection(pointSetToLabelHierarchyFilter->GetOutputPort());
	//Use depth buffer
	this->labelMapper->SetUseDepthBuffer(this->useDepthBuffer);
	//Add the mappers to the actors.
	this->labelActor->SetMapper(labelMapper);

}
//----------------------------------------------------------------------------
void KronosRepresentation::FlightRepresentation(vtkPolyData* input) {
    //Generate the label hierarchy and filter the points according to the priorities.
    this->pointSetToLabelHierarchyFilter = vtkSmartPointer<vtkPointSetToLabelHierarchy>::New();
    this->pointSetToLabelHierarchyFilter->SetInputData(input);
    this->pointSetToLabelHierarchyFilter->SetLabelArrayName("airlines");
    this->pointSetToLabelHierarchyFilter->SetPriorityArrayName("priorities");
    this->pointSetToLabelHierarchyFilter->Update();
    
    //Create the labelmapper
    this->labelMapper = vtkSmartPointer<KronosLabelMapper>::New();
    this->labelMapper->SetInputConnection(pointSetToLabelHierarchyFilter->GetOutputPort());
    this->labelMapper->SetUseDepthBuffer(this->useDepthBuffer);
    
    //Add the mappers to the actors.
    this->labelActor->SetMapper(labelMapper);
    
}
//----------------------------------------------------------------------------
void KronosRepresentation::SetVisibility(bool val) {
	//Set visibility of the actors and superclass.
	this->Superclass::SetVisibility(val);
	this->labelActor->SetVisibility(val);
}
//----------------------------------------------------------------------------
bool KronosRepresentation::AddToView(vtkView* view) {
	//Adds the actors to the view.
	vtkPVRenderView* rview = vtkPVRenderView::SafeDownCast(view);
	if (rview) {
		rview->GetRenderer()->AddActor(this->labelActor);

	}
	return this->Superclass::AddToView(view);
}
bool KronosRepresentation::RemoveFromView(vtkView* view) {
	//Removes the actors from the view.
	vtkPVRenderView* rview = vtkPVRenderView::SafeDownCast(view);
	if (rview) {
		rview->GetRenderer()->RemoveActor(this->labelActor);
	}
	return this->Superclass::RemoveFromView(view);
}
//----------------------------------------------------------------------------
int KronosRepresentation::FillInputPortInformation(int port, vtkInformation* info) {
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
