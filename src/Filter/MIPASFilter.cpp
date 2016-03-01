#include <vtkDataObjectAlgorithm.h>
#include "MIPASFilter.h"
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
#include <vtkFloatArray.h>
#include <vtkUnsignedIntArray.h>


vtkStandardNewMacro(MIPASFilter);
MIPASFilter::MIPASFilter() {}
MIPASFilter::~MIPASFilter() {}

int MIPASFilter::RequestInformation(vtkInformation* request,
                                   vtkInformationVector** inputVector,
                                   vtkInformationVector* outputVector) {
	return 1;
}

QList<Data::Type> MIPASFilter::getCompatibleDataTypes() {
	return (QList<Data::Type>());
}

bool MIPASFilter::evaluatePoint(int pointIndex, Coordinate coordinate,
                               vtkPointData* pointData) {
	vtkSmartPointer<vtkDoubleArray> timeArray = vtkDoubleArray::SafeDownCast(
	            pointData->GetAbstractArray("time"));

	vtkSmartPointer<vtkFloatArray> altitudeArray = vtkFloatArray::SafeDownCast(
	            pointData->GetAbstractArray("altitude"));

	vtkSmartPointer<vtkUnsignedIntArray> orbitArray = vtkUnsignedIntArray::SafeDownCast(
	            pointData->GetAbstractArray("orbit_id"));

	if (!timeArray || !altitudeArray || !orbitArray) {
		this->fail("One of the arrays (\"time\", \"altitude\" or \"orbit_id\") seems to be invalid or missing.");
		return 0;
	}

	double time = timeArray->GetTuple1(pointIndex);
	float altitude = altitudeArray->GetTuple1(pointIndex);
	int orbit = orbitArray->GetTuple1(pointIndex);

	return (this->lowerTimeLimit <= time && time <= this->upperTimeLimit)
	       && (this->lowerAltitudeLimit <= altitude && altitude <= this->upperAltitudeLimit) &&
	       (this->lowerOrbitLimit <= orbit && orbit <= this->upperOrbitLimit);
}

void MIPASFilter::SetInputConnection(vtkAlgorithmOutput* input) {
	this->Superclass::SetInputConnection(input);
}

void MIPASFilter::setTimeThreshold(double lowerLimit, double upperLimit) {
	this->lowerTimeLimit = lowerLimit;
	this->upperTimeLimit = upperLimit;
	this->Modified();
}

void MIPASFilter::setAltitudeThreshold(float lowerLimit, float upperLimit ) {
	this->lowerAltitudeLimit = lowerLimit;
	this->upperAltitudeLimit = upperLimit;
	this->Modified();
}

void MIPASFilter::setOrbitThreshold(float lowerLimit, float upperLimit) {
	this->lowerOrbitLimit = lowerLimit;
	this->upperOrbitLimit = upperLimit;
	this->Modified();
}
