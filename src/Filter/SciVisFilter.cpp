#include <vtkDataObjectAlgorithm.h>
#include "SciVisFilter.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include <vtkThreshold.h>
#include "vtkPoints.h"
#include "vtkDataSetAlgorithm.h"
#include <vtkInformation.h>
#include "vtkInformationVector.h"
#include <vtkIndent.h>
#include <vtkDataSet.h>
#include <vtkUnstructuredGridAlgorithm.h>
#include <iostream>
#include <vtkPolyData.h>

vtkStandardNewMacro(SciVisFilter);
SciVisFilter::SciVisFilter() {}

SciVisFilter::~SciVisFilter() {}


void SciVisFilter::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent) ;
}

int AbstractSelectionFilter::RequestData(vtkInformation* info,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector) {
}

vtkInformation* inputInformation = inputVector[0]->GetInformationObject(0);
vtkPolyData* inputData = vtkPolyData::SafeDownCast(inputInformation->Get(
                             vtkDataObject::DATA_OBJECT()));

vtkInformation* outputInformation = outputVector->GetInformationObject(0);
vtkPolyData* output = vtkPolyData::SafeDownCast(outputInformation->Get(
                          vtkDataObject::DATA_OBJECT()));




void SciVisFilter::setLowerMIPAS(double lowerLimit){
  //vtkThreshold::LowerThreshold = lowerLimit;
  this->Modified();
}

void SciVisFilter::setUpperMIPAS(double upperLimit){
 //vtkThreshold::UpperThreshold = upperLimit;
 this->Modified();
}


void SciVisFilter::setLowerAIRS(double lowerLimit){
  //vtkThreshold::LowerThreshold = lowerLimit;
  this->Modified();
}

void SciVisFilter::setUpperAIRS(double upperLimit){
 //vtkThreshold::UpperThreshold = upperLimit;
 this->Modified();
}

void SciVisFilter::setLowerCLaMS(double lowerLimit){
  //vtkThreshold::LowerThreshold = lowerLimit;
  this->Modified();
}

void SciVisFilter::setUpperCLaMS(double upperLimit){
 //vtkThreshold::UpperThreshold = upperLimit;
 this->Modified();
}
