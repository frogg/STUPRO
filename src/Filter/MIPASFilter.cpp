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
#include <vtkUnsignedCharArray.h>


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
	vtkSmartPointer<vtkUnsignedCharArray> profileArray = vtkUnsignedCharArray::SafeDownCast(
	            pointData->GetAbstractArray("profile_id"));
	vtkSmartPointer<vtkUnsignedCharArray> detectionArray = vtkUnsignedCharArray::SafeDownCast(
	            pointData->GetAbstractArray("detection"));

	if (!timeArray || !altitudeArray || !orbitArray || !profileArray || !detectionArray) {
		return false;
	}

	double time = timeArray->GetTuple1(pointIndex);
	float altitude = altitudeArray->GetTuple1(pointIndex);
	int orbit = orbitArray->GetTuple1(pointIndex);
	char detection = detectionArray->GetTuple1(pointIndex);
	char profile = profileArray->GetTuple1(pointIndex);

	return (this->lowerTimeLimit <= time && time <= this->upperTimeLimit)
	       && (this->lowerAltitudeLimit <= altitude && altitude <= this->upperAltitudeLimit) &&
	       (this->lowerOrbitLimit <= orbit && orbit <= this->upperOrbitLimit)
	       && (this->lowerDetectionLimit <= detection && detection <= this->upperDetectionLimit)
	       && (this->lowerProfileLimit <= profile && profile <= this->upperProfileLimit);
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

void MIPASFilter::setDetectionThreshold(double lowerLimit, double upperLimit) {
	this->lowerDetectionLimit = lowerLimit;
	this->upperDetectionLimit = upperLimit;
	this->Modified();
}

void MIPASFilter::setProfileThreshold(double lowerLimit, double upperLimit) {
	this->lowerProfileLimit = lowerLimit;
	this->upperProfileLimit = upperLimit;
	this->Modified();
}
