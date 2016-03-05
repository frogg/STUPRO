#include <Filter/TemperatureThresholdFilter.h>

#include <Reader/DataReader/Data.hpp>

#include <vtkObjectFactory.h>
#include <vtkFloatArray.h>

TemperatureThresholdFilter::TemperatureThresholdFilter() { }
TemperatureThresholdFilter::~TemperatureThresholdFilter() { }

vtkStandardNewMacro(TemperatureThresholdFilter);

QList<Data::Type> TemperatureThresholdFilter::getCompatibleDataTypes() {
	return (QList<Data::Type>() << Data::TEMPERATURE);
}

bool TemperatureThresholdFilter::evaluatePoint(int pointIndex, Coordinate coordinate,
        vtkPointData* pointData) {
	vtkSmartPointer<vtkFloatArray> temperatureArray = vtkFloatArray::SafeDownCast(
	            pointData->GetArray("temperatures"));

	if (!temperatureArray) {
		return false;
	}

	float temperature = temperatureArray->GetTuple1(pointIndex);

	return temperature >= this->lowerLimit && temperature <= this->upperLimit;
}

void TemperatureThresholdFilter::SetInputConnection(vtkAlgorithmOutput* input) {
	this->Superclass::SetInputConnection(input);
}

void TemperatureThresholdFilter::setThreshold(double lowerLimit, double upperLimit) {
	this->lowerLimit = lowerLimit;
	this->upperLimit = upperLimit;
	this->Modified();
}
