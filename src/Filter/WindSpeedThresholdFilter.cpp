#include <Filter/WindSpeedThresholdFilter.h>

#include <Reader/DataReader/Data.hpp>

#include <vtkObjectFactory.h>
#include <vtkFloatArray.h>

WindSpeedThresholdFilter::WindSpeedThresholdFilter() { }
WindSpeedThresholdFilter::~WindSpeedThresholdFilter() { }

vtkStandardNewMacro(WindSpeedThresholdFilter);

QList<Data::Type> WindSpeedThresholdFilter::getCompatibleDataTypes() {
	return (QList<Data::Type>() << Data::WIND);
}

bool WindSpeedThresholdFilter::evaluatePoint(int pointIndex, Coordinate coordinate,
        vtkPointData* pointData) {
	vtkSmartPointer<vtkFloatArray> speedArray = vtkFloatArray::SafeDownCast(
	            pointData->GetArray("speeds"));

	if (!speedArray) {
		return false;
	}

	float speed = speedArray->GetTuple1(pointIndex);

	return speed >= this->lowerLimit && speed <= this->upperLimit;
}

void WindSpeedThresholdFilter::SetInputConnection(vtkAlgorithmOutput* input) {
	this->Superclass::SetInputConnection(input);
}

void WindSpeedThresholdFilter::setThreshold(double lowerLimit, double upperLimit) {
	this->lowerLimit = lowerLimit;
	this->upperLimit = upperLimit;
	this->Modified();
}
