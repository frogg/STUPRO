#include <vtkDataObjectAlgorithm.h>
#include "CLaMSFilter.h"
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


vtkStandardNewMacro(CLaMSFilter);
CLaMSFilter::CLaMSFilter() {}
CLaMSFilter::~CLaMSFilter() {}

int CLaMSFilter::RequestInformation(vtkInformation* request,
                                    vtkInformationVector** inputVector,
                                    vtkInformationVector* outputVector) {
	return 1;
}

QList<Data::Type> CLaMSFilter::getCompatibleDataTypes() {
	return (QList<Data::Type>());
}

bool CLaMSFilter::evaluatePoint(int pointIndex, Coordinate coordinate,
                                vtkPointData* pointData) {
	vtkSmartPointer<vtkDoubleArray> timeArray = vtkDoubleArray::SafeDownCast(
	            pointData->GetAbstractArray("time"));
	vtkSmartPointer<vtkFloatArray> altitudeArray = vtkFloatArray::SafeDownCast(
	            pointData->GetAbstractArray("altitude"));
	vtkSmartPointer<vtkFloatArray> temperatureArray = vtkFloatArray::SafeDownCast(
	            pointData->GetAbstractArray("temperature"));
	vtkSmartPointer<vtkFloatArray> pressureArray = vtkFloatArray::SafeDownCast(
	            pointData->GetAbstractArray("pressure"));
	vtkSmartPointer<vtkFloatArray> vorticityArray = vtkFloatArray::SafeDownCast(
	            pointData->GetAbstractArray("pot_vorticity"));
	vtkSmartPointer<vtkFloatArray> potTemperatureArray = vtkFloatArray::SafeDownCast(
	            pointData->GetAbstractArray("pot_temperature"));

	if (!timeArray || !altitudeArray || !temperatureArray || !pressureArray || !vorticityArray
	        || !potTemperatureArray) {
		return false;
	}

	double time = timeArray->GetTuple1(pointIndex);
	float altitude = altitudeArray->GetTuple1(pointIndex);
	float temperature = temperatureArray->GetTuple1(pointIndex);
	float pressure = pressureArray->GetTuple1(pointIndex);
	float vorticity = vorticityArray->GetTuple1(pointIndex);
	float potTemperature = potTemperatureArray->GetTuple1(pointIndex);

	return (this->lowerTimeLimit <= time && time <= this->upperTimeLimit)
	       && (this->lowerAltitudeLimit <= altitude && altitude <= this->upperAltitudeLimit) &&
	       (this->lowerTemperatureLimit <= temperature && temperature <= this->upperTemperatureLimit)
	       && (this->lowerPressureLimit <= pressure && pressure <= this->upperPressureLimit)
	       && (this->lowerVorticityLimit <= vorticity && vorticity <= this->upperVorticityLimit)
	       && (this->lowerPotTemperatureLimit <= potTemperature
	           && potTemperature <= this->upperPotTemperatureLimit);
}

void CLaMSFilter::SetInputConnection(vtkAlgorithmOutput* input) {
	this->Superclass::SetInputConnection(input);
}

void CLaMSFilter::setTimeThreshold(double lowerLimit, double upperLimit) {
	this->lowerTimeLimit = lowerLimit;
	this->upperTimeLimit = upperLimit;
	this->Modified();
}

void CLaMSFilter::setAltitudeThreshold(float lowerLimit, float upperLimit ) {
	this->lowerAltitudeLimit = lowerLimit;
	this->upperAltitudeLimit = upperLimit;
	this->Modified();
}

void CLaMSFilter::setTemperatureThreshold(float lowerLimit, float upperLimit) {
	this->lowerTemperatureLimit = lowerLimit;
	this->upperTemperatureLimit = upperLimit;
	this->Modified();
}

void CLaMSFilter::setPressureThreshold(double lowerLimit, double upperLimit) {
	this->lowerPressureLimit = lowerLimit;
	this->upperPressureLimit = upperLimit;
	this->Modified();
}

void CLaMSFilter::setVorticityThreshold(double lowerLimit, double upperLimit) {
	this->lowerVorticityLimit = lowerLimit;
	this->upperVorticityLimit = upperLimit;
	this->Modified();
}

void CLaMSFilter::setPotTemperatureThreshold(double lowerLimit, double upperLimit) {
	this->lowerPotTemperatureLimit = lowerLimit;
	this->upperPotTemperatureLimit = upperLimit;
	this->Modified();
}
