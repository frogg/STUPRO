/*=========================================================================

  Program:   ParaView
  Module:    vtkMySpecialPolyDataMapper.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMySpecialPolyDataMapper.h"

#include "vtkObjectFactory.h"
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkIntArray.h>
#include <vtkLabelPlacementMapper.h>
#include <vtkPointData.h>
#include <vtkPointSetToLabelHierarchy.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
vtkStandardNewMacro(vtkMySpecialPolyDataMapper);
//----------------------------------------------------------------------------
vtkMySpecialPolyDataMapper::vtkMySpecialPolyDataMapper()
{
    // Generate the label hierarchy.
    vtkSmartPointer<vtkPointSetToLabelHierarchy> pointSetToLabelHierarchyFilter =
    vtkSmartPointer<vtkPointSetToLabelHierarchy>::New();
    pointSetToLabelHierarchyFilter->SetInputConnection(
    pointSource->GetOutputPort());
    pointSetToLabelHierarchyFilter->SetLabelArrayName("labels");
    pointSetToLabelHierarchyFilter->SetPriorityArrayName("sizes");
    pointSetToLabelHierarchyFilter->Update();
    
}

//----------------------------------------------------------------------------
vtkMySpecialPolyDataMapper::~vtkMySpecialPolyDataMapper()
{
}

//----------------------------------------------------------------------------
void vtkMySpecialPolyDataMapper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
//----------------------------------------------------------------------------
void vtkMySpecialPolyDataMapper::Render(vtkRenderer* ren, vtkActor* act)
{
    // Create a mapper and actor for the labels.
    vtkSmartPointer<vtkLabelPlacementMapper> labelMapper =
    vtkSmartPointer<vtkLabelPlacementMapper>::New();
    labelMapper->SetInputConnection(
    pointSetToLabelHierarchyFilter->GetOutputPort());
    vtkSmartPointer<vtkActor2D> labelActor =
    vtkSmartPointer<vtkActor2D>::New();
    labelActor->SetMapper(labelMapper);
    ren->AddActor(labelActor);
}




