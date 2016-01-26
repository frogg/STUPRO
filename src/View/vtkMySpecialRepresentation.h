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
// .NAME vtkMySpecialRepresentation
// .SECTION Description
//

#ifndef __vtkMySpecialRepresentation_h
#define __vtkMySpecialRepresentation_h

#include "vtkGeometryRepresentationWithFaces.h"
#include "vtkPVClientServerCoreRenderingModule.h" //needed for exports
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkGeometryRepresentation.h"
#include "vtkLabelPlacementMapper.h"
#include "vtkSmartPointer.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkPointSource.h"
#include "vtkLabelPlacementMapper.h"
#include "vtkActor2D.h"
#include "vtkLabelPlacementMapper.h"
#include "vtkPointSetToLabelHierarchy.h"
class VTK_EXPORT vtkMySpecialRepresentation : public vtkGeometryRepresentationWithFaces
{
public:
  static vtkMySpecialRepresentation* New();
    
  vtkTypeMacro(vtkMySpecialRepresentation, vtkGeometryRepresentationWithFaces);
    
  void PrintSelf(ostream& os, vtkIndent indent);
  void SetVisibility(bool val);
  
//BTX
protected:
    vtkMySpecialRepresentation();
    ~vtkMySpecialRepresentation();
    virtual int RequestData(vtkInformation*,
                            vtkInformationVector**, vtkInformationVector*);
    
    virtual bool AddToView(vtkView* view);
    virtual bool RemoveFromView(vtkView* view);
    int FillInputPortInformation(int, vtkInformation *info) override;
    
private:
    
    vtkMySpecialRepresentation(const vtkMySpecialRepresentation&); // Not implemented
    void operator=(const vtkMySpecialRepresentation&); // Not implemented
    // Create a dummy point set.
    vtkSmartPointer<vtkPolyDataMapper> pointMapper;
    vtkSmartPointer<vtkActor> pointActor;
    vtkSmartPointer<vtkLabelPlacementMapper> labelMapper;
    vtkSmartPointer<vtkActor2D> labelActor;
    vtkSmartPointer<vtkPointSetToLabelHierarchy> pointSetToLabelHierarchyFilter;


//ETX
};

#endif
