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
#include "vtkDataSet.h"
#include "vtkDoubleArray.h"
#include "vtkGenericCell.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkTensor.h"

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
  // get the info objects
vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
vtkInformation *outInfo = outputVector->GetInformationObject(0);

// get the input and output
vtkDataSet *input = vtkDataSet::SafeDownCast(
  inInfo->Get(vtkDataObject::DATA_OBJECT()));
vtkDataSet *output = vtkDataSet::SafeDownCast(
  outInfo->Get(vtkDataObject::DATA_OBJECT()));

  output->CopyStructure( input );
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
