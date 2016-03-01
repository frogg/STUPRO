#include <vtkDataObjectAlgorithm.h>
#include "AIRSFilter.h"
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
#include <vtkDataObjectAlgorithm.h>
#include <vtkDoubleArray.h>


vtkStandardNewMacro(AIRSFilter);
AIRSFilter::AIRSFilter() {}
AIRSFilter::~AIRSFilter() {}

int AIRSFilter::RequestInformation(vtkInformation* request,
                       vtkInformationVector** inputVector,
                       vtkInformationVector* outputVector){
    return 1;
}

QList<Data::Type> AIRSFilter::getCompatibleDataTypes() {
    return (QList<Data::Type>());
}

bool AIRSFilter::evaluatePoint(int pointIndex, Coordinate coordinate,
                                 vtkPointData* pointData) {
    vtkSmartPointer<vtkDoubleArray> timeArray = vtkDoubleArray::SafeDownCast(pointData->GetAbstractArray("time"));
    double time = timeArray->GetTuple1(pointIndex);
    return this->lowerTimeLimit <= time && time <= this->upperTimeLimit;
}

void AIRSFilter::SetInputConnection(vtkAlgorithmOutput* input) {
    this->Superclass::SetInputConnection(input);
}

void AIRSFilter::setTimeThreshold(double lowerLimit, double upperLimit){
    this->lowerTimeLimit = lowerLimit;
    this->upperTimeLimit = upperLimit;
    this->Modified();
}

