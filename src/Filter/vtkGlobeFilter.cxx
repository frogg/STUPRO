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
vtkInformationVector *temp;
int tempvar=2;

int vtkGlobeFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
    temp=outputVector;
      std::cout << "dataRequested" << tempvar<< endl;
  // get the info objects
//vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
vtkInformation *outInfo = outputVector->GetInformationObject(0);

// get the input and output
//vtkPolyData *input = vtkPolyData::SafeDownCast(
//  inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData *output = vtkPolyData::SafeDownCast(
  outInfo->Get(vtkDataObject::DATA_OBJECT()));

    int numberOfQuadsRight=20;
    int numberOfQuadsUp=20;
    
    int nmbPoints=5*numberOfQuadsRight*numberOfQuadsUp;

    vtkCellArray *newPolys;
    newPolys = vtkCellArray::New();
    newPolys->Allocate(newPolys->EstimateSize(nmbPoints, 3));
    
    vtkPoints *newPoints;
    newPoints = vtkPoints::New();
    vtkFloatArray *newNormals;
    newPoints->Allocate(nmbPoints);
    newNormals = vtkFloatArray::New();
    newNormals->SetNumberOfComponents(3);
    newNormals->Allocate(3*nmbPoints);
    newNormals->SetName("Normals");

    vtkIdType pts[4];
    for (int i=0; i<numberOfQuadsRight; i++) {
        for (int j=0; j<numberOfQuadsUp; j++) {

            
    pts[0]  = newPoints->InsertNextPoint(-1.0+2.0*i, 0.0+2.0*j, sin(-1.0+2.0*i)*2*tempvar);
    pts[1]  = newPoints->InsertNextPoint(1.0+2.0*i, 0.0+2.0*j, sin(1.0+2.0*i)*2*tempvar);
    pts[2]  = newPoints->InsertNextPoint(0.0+2.0*i, 1.0+2.0*j, sin(0.0+2.0*i)*2*tempvar);
    newPolys->InsertNextCell(3, pts);
    newNormals->InsertTuple3(pts[0], 1.0, 0.0, 0.0);
    newNormals->InsertTuple3(pts[1], 1.0, 0.0, 0.0);
    newNormals->InsertTuple3(pts[2], 1.0, 0.0, 0.0);
            
    pts[1]  = newPoints->InsertNextPoint(-1.0+2.0*i, 2.0+2.0*j, sin(-1.0+2.0*i)*2*tempvar);
    newPolys->InsertNextCell(3, pts);
    newNormals->InsertTuple3(pts[1], 1.0, 0.0, 0.0);
            
    pts[0]  = newPoints->InsertNextPoint(1.0+2.0*i, 2.0+2.0*j, sin(1.0+2.0*i)*2*tempvar);
    newPolys->InsertNextCell(3, pts);
    newNormals->InsertTuple3(pts[0], 1.0, 0.0, 0.0);
            
    pts[1]  = newPoints->InsertNextPoint(1.0+2.0*i, 0.0+2.0*j, sin(1.0+2.0*i)*2*tempvar);
    newNormals->InsertTuple3(pts[0], 1.0, 0.0, 0.0);
    newPolys->InsertNextCell(3, pts);
        }
    
    }
  
   // output->CopyStructure( input );
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
void vtkGlobeFilter::reloadStuff(void){
    tempvar++;

    RequestData(nullptr, nullptr, temp);
        this->UpdateDataObject();
    this->Update();

    this->UpdateInformation();
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
