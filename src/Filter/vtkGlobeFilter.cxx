/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGlobeFilter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkGlobeFilter.h"

#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkGenericCell.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkTensor.h"
#include "vtkPolyData.h"
#include <vtkCellArray.h>
#include "vtkPointData.h"
#include "vtkPoints.h"
#include <math.h>

vtkStandardNewMacro(vtkGlobeFilter);

vtkGlobeFilter::vtkGlobeFilter()
{
  std::cout << "Hello" << endl;

}

int vtkGlobeFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
      std::cout << "dataRequested" << endl;
  // get the info objects
vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
vtkInformation *outInfo = outputVector->GetInformationObject(0);

// get the input and output
vtkPolyData *input = vtkPolyData::SafeDownCast(
  inInfo->Get(vtkDataObject::DATA_OBJECT()));
vtkPolyData *output = vtkPolyData::SafeDownCast(
  outInfo->Get(vtkDataObject::DATA_OBJECT()));

    int nmbPoints=100;

    vtkCellArray *newPolys;
    newPolys = vtkCellArray::New();
    newPolys->Allocate(newPolys->EstimateSize(nmbPoints*nmbPoints, 3));
    
    vtkPoints *newPoints;
    newPoints = vtkPoints::New();
    vtkFloatArray *newNormals;
    newPoints->Allocate(nmbPoints*nmbPoints);
    newNormals = vtkFloatArray::New();
    newNormals->SetNumberOfComponents(3);
    newNormals->Allocate(3*nmbPoints*nmbPoints);
    newNormals->SetName("Normals");
    newPoints->InsertNextPoint(1, 0.0, 0.0);
    newNormals->InsertTuple3(1, 0.0, 0.0, 0.0);
    vtkIdType pts[4];
    for (int i=0; i<nmbPoints; i++) {
        for (int j=0 ; j<nmbPoints; j++) {
            pts[0]=j;
            pts[1]=i;
            pts[2]=0.0;
            newPolys->InsertNextCell(3, pts);
        }
    }


  
  //output->CopyStructure( input );
    newPoints->Squeeze();
    output->SetPoints(newPoints);
    newPoints->Delete();
    
    newNormals->Squeeze();
    output->GetPointData()->SetNormals(newNormals);
    newNormals->Delete();
    
    newPolys->Squeeze();
    output->SetPolys(newPolys);
    newPolys->Delete();

    return 1;
}

const char *vtkGlobeFilter::GetVectorModeAsString(void)
{
  if ( this->VectorMode == VTK_VECTOR_MODE_PASS_VECTORS )
    {
    return "PassVectors";
    }
  else if ( this->VectorMode == VTK_VECTOR_MODE_COMPUTE_GRADIENT )
    {
    return "ComputeGradient";
    }
  else //VTK_VECTOR_MODE_COMPUTE_VORTICITY
    {
    return "ComputeVorticity";
    }
}

const char *vtkGlobeFilter::GetTensorModeAsString(void)
{
  if ( this->TensorMode == VTK_TENSOR_MODE_PASS_TENSORS )
    {
    return "PassTensors";
    }
  else if ( this->TensorMode == VTK_TENSOR_MODE_COMPUTE_GRADIENT )
    {
    return "ComputeGradient";
    }
  else //VTK_TENSOR_MODE_COMPUTE_STRAIN
    {
    return "ComputeVorticity";
    }
}

void vtkGlobeFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

os << indent << "Vector Mode: " << this->GetVectorModeAsString()
   << endl;

os << indent << "Tensor Mode: " << this->GetTensorModeAsString()
   << endl;
}
