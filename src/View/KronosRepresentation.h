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
// .NAME KronosRepresentation
// .SECTION Description
//

#ifndef __KronosRepresentation_h
#define __KronosRepresentation_h

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
class VTK_EXPORT KronosRepresentation : public vtkGeometryRepresentationWithFaces
{
public:
  static KronosRepresentation* New();

  vtkTypeMacro(KronosRepresentation, vtkGeometryRepresentationWithFaces);

  void PrintSelf(ostream& os, vtkIndent indent);
  void SetVisibility(bool val);

//BTX
protected:
    KronosRepresentation();
    ~KronosRepresentation();
    virtual int RequestData(vtkInformation*,
                            vtkInformationVector**, vtkInformationVector*);

    virtual bool AddToView(vtkView* view);
    virtual bool RemoveFromView(vtkView* view);
    int FillInputPortInformation(int, vtkInformation *info) override;

private:

    KronosRepresentation(const KronosRepresentation&); // Not implemented
    void operator=(const KronosRepresentation&); // Not implemented
    // Create a dummy point set.
    vtkSmartPointer<vtkPolyDataMapper> pointMapper;
    vtkSmartPointer<vtkActor> pointActor;
    vtkSmartPointer<vtkLabelPlacementMapper> labelMapper;
    vtkSmartPointer<vtkActor2D> labelActor;
    vtkSmartPointer<vtkPointSetToLabelHierarchy> pointSetToLabelHierarchyFilter;


//ETX
};

#endif
