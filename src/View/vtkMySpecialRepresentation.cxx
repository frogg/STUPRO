/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile$

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMySpecialRepresentation.h"

#include "vtkObjectFactory.h"
#include "vtkMySpecialPolyDataMapper.h"

#include <vtkPointSetToLabelHierarchy.h>
#include <vtkLabelPlacementMapper.h>
#include "vtkPVRenderView.h"
#include "vtkRenderer.h"
#include "vtkStringArray.h"
#include "vtkPointSetToLabelHierarchy.h"
#include "vtkPointData.h"
vtkStandardNewMacro(vtkMySpecialRepresentation);
//----------------------------------------------------------------------------
vtkMySpecialRepresentation::vtkMySpecialRepresentation()
{
    // Create a mapper and actor for the points.
    vtkSmartPointer<vtkPolyDataMapper> pointMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();

    // Create a dummy DataSet.
    this->dummyPointSource =
    vtkSmartPointer<vtkPointSource>::New();
    this->dummyPointSource->SetNumberOfPoints(6);
    this->dummyPointSource->Update();
    // Add label array.
    vtkSmartPointer<vtkStringArray> labels =
    vtkSmartPointer<vtkStringArray>::New();
    labels->SetNumberOfValues(6);
    labels->SetName("labels");
    labels->SetValue(0, "Priority 10");
    labels->SetValue(1, "Priority 7");
    labels->SetValue(2, "Priority 6");
    labels->SetValue(3, "Priority 4");
    labels->SetValue(4, "Priority 4");
    labels->SetValue(5, "Priority 4");
    this->dummyPointSource->GetOutput()->GetPointData()->AddArray(labels);
    
    // Create a mapper and actor for the points.
    this->pointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    this->pointMapper->SetInputConnection(this->dummyPointSource->GetOutputPort());
    
    this->pointActor = vtkSmartPointer<vtkActor>::New();
    this->pointActor->SetMapper(this->pointMapper);
    
    // Generate the label hierarchy.
    vtkSmartPointer<vtkPointSetToLabelHierarchy> pointSetToLabelHierarchyFilter =
    vtkSmartPointer<vtkPointSetToLabelHierarchy>::New();
    pointSetToLabelHierarchyFilter->SetInputConnection(
                                                       this->dummyPointSource->GetOutputPort());
    pointSetToLabelHierarchyFilter->SetLabelArrayName("labels");
    pointSetToLabelHierarchyFilter->SetPriorityArrayName("sizes");
    pointSetToLabelHierarchyFilter->Update();
    
    
    // Create a mapper and actor for the labels.
    labelMapper = vtkSmartPointer<vtkLabelPlacementMapper>::New();
    this->labelMapper->SetInputConnection(pointSetToLabelHierarchyFilter->GetOutputPort());
    
    this->labelActor = vtkSmartPointer<vtkActor2D>::New();
    this->labelActor->SetMapper(labelMapper);
    
    this->SetupDefaults();
}

//----------------------------------------------------------------------------
vtkMySpecialRepresentation::~vtkMySpecialRepresentation()
{
}

int vtkMySpecialRepresentation::RequestData(vtkInformation* request,
                                            vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
    
    if (inputVector[1]->GetNumberOfInformationObjects()==1)
    {
        this->pointMapper->SetInputConnection(1, this->GetInternalOutputPort(1));

    }
    else
    {
        this->pointMapper->SetInputConnection(1, this->dummyPointSource->GetOutputPort());
    }
    
    cout << "=================RD==================" << endl;
    return 1;
}

//----------------------------------------------------------------------------
bool vtkMySpecialRepresentation::AddToView(vtkView* view)
{
    vtkPVRenderView* rview = vtkPVRenderView::SafeDownCast(view);
    if (rview)
    {
        rview->GetRenderer()->AddActor(this->pointActor);
        rview->GetRenderer()->AddActor(this->labelActor);
    }
    return this->Superclass::AddToView(view);
}
//----------------------------------------------------------------------------
void vtkMySpecialRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
