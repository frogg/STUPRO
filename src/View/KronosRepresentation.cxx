#include "KronosRepresentation.h"

#include "vtkObjectFactory.h"


#include <vtkPointSetToLabelHierarchy.h>
#include <vtkLabelPlacementMapper.h>
#include "vtkPVRenderView.h"
#include "vtkRenderer.h"
#include "vtkStringArray.h"
#include "vtkPointSetToLabelHierarchy.h"
#include "vtkPointData.h"

vtkStandardNewMacro(KronosRepresentation);
//----------------------------------------------------------------------------
KronosRepresentation::KronosRepresentation()
{
    
    this->SetNumberOfInputPorts(1);
    this->SetNumberOfOutputPorts(0);
    // Create a mapper and actor for the points.
    this->pointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();

    this->pointActor = vtkSmartPointer<vtkActor>::New();

    this->pointActor->SetMapper(this->pointMapper);

    // Generate the label hierarchy.
    this->pointSetToLabelHierarchyFilter = vtkSmartPointer<vtkPointSetToLabelHierarchy>::New();

    // Create a mapper and actor for the labels.
    this->labelMapper = vtkSmartPointer<vtkLabelPlacementMapper>::New();
    this->labelMapper->SetInputConnection(pointSetToLabelHierarchyFilter->GetOutputPort());
    this->labelActor = vtkSmartPointer<vtkActor2D>::New();
    this->labelActor->SetMapper(labelMapper);

    this->labelMapper->UseDepthBufferOn();
    
    this->SetVisibility(false);
}

//----------------------------------------------------------------------------
KronosRepresentation::~KronosRepresentation()
{
    this->pointMapper->Delete();
    this->pointActor->Delete();
    this->pointSetToLabelHierarchyFilter->Delete();
    this->labelMapper->Delete();
    this->labelActor->Delete();

}
//----------------------------------------------------------------------------
int KronosRepresentation::RequestData(vtkInformation* request,
                                            vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
    if(inputVector[0]->GetNumberOfInformationObjects()==1){
        vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
        vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
        
        this->pointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        this->pointMapper->SetInputData(input);
        
        // Generate the label hierarchy.
        this->pointSetToLabelHierarchyFilter = vtkSmartPointer<vtkPointSetToLabelHierarchy>::New();
        this->pointSetToLabelHierarchyFilter->SetInputData(input);
        this->pointSetToLabelHierarchyFilter->SetLabelArrayName("names");
        this->pointSetToLabelHierarchyFilter->SetPriorityArrayName("priorities");
        this->pointSetToLabelHierarchyFilter->Update();
        
        this->labelMapper = vtkSmartPointer<vtkLabelPlacementMapper>::New();
        this->labelMapper->SetInputConnection(pointSetToLabelHierarchyFilter->GetOutputPort());
        this->labelMapper->UseDepthBufferOn();
        
        this->pointActor->SetMapper(this->pointMapper);
  
        this->labelActor->SetMapper(labelMapper);

        
        inputVector[0]->Remove(inInfo);
    }
    return this->Superclass::RequestData(request, inputVector, outputVector);

}

//----------------------------------------------------------------------------
void KronosRepresentation::SetVisibility(bool val)
{
    this->Superclass::SetVisibility(val);
    this->labelActor->SetVisibility(val?  1 : 0);
    this->pointActor->SetVisibility(val?  1 : 0);
}

//----------------------------------------------------------------------------
bool KronosRepresentation::AddToView(vtkView* view)
{
    vtkPVRenderView* rview = vtkPVRenderView::SafeDownCast(view);
    if (rview)
    {
        rview->GetRenderer()->AddActor(this->pointActor);
        rview->GetRenderer()->AddActor(this->labelActor);
    }
    return this->Superclass::AddToView(view);
}
bool KronosRepresentation::RemoveFromView(vtkView* view)
{
    vtkPVRenderView* rview = vtkPVRenderView::SafeDownCast(view);
    if (rview)
    {
        rview->GetRenderer()->RemoveActor(this->pointActor);
        rview->GetRenderer()->RemoveActor(this->labelActor);
    }
    return this->Superclass::RemoveFromView(view);
}
//----------------------------------------------------------------------------
int KronosRepresentation::FillInputPortInformation(int, vtkInformation *info) {
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
    return 1;
}
//----------------------------------------------------------------------------
void KronosRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
