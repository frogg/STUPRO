#include <Filter/PrecipitationTypeFilter.h>

#include <Reader/DataReader/Data.hpp>

#include <vtkObjectFactory.h>
#include <vtkTypeInt32Array.h>

PrecipitationTypeFilter::PrecipitationTypeFilter() {
	// Display all precipitation types by default
	for (int i = PrecipitationDataPoint::NONE; i <= PrecipitationDataPoint::HAIL; i++) {
		this->precipitationTypeVisibilities.insert(static_cast<PrecipitationDataPoint::PrecipitationType>
		        (i), true);
	}
}
PrecipitationTypeFilter::~PrecipitationTypeFilter() { }

vtkStandardNewMacro(PrecipitationTypeFilter);

QList<Data::Type> PrecipitationTypeFilter::getCompatibleDataTypes() {
	return (QList<Data::Type>() << Data::PRECIPITATION);
}

bool PrecipitationTypeFilter::evaluatePoint(int pointIndex, Coordinate coordinate,
        vtkPointData* pointData) {
	vtkSmartPointer<vtkTypeInt32Array> precipitationTypeArray = vtkTypeInt32Array::SafeDownCast(
	            pointData->GetArray("precipitationTypes"));
       //precipitationTypeVisibilities contains a list with all precipitationTypes that should be visible
       //(precipitationTypeArray->GetTuple1(pointIndex)) returns the percipitation type for the given point index
       //we check if the percipitation type for the given point index is in precipitationTypeVisibilities
	return this->precipitationTypeVisibilities[static_cast<PrecipitationDataPoint::PrecipitationType>
	        (precipitationTypeArray->GetTuple1(pointIndex))];
}

void PrecipitationTypeFilter::SetInputConnection(vtkAlgorithmOutput* input) {
	this->Superclass::SetInputConnection(input);
}

void PrecipitationTypeFilter::displayPrecipitationType(PrecipitationDataPoint::PrecipitationType
        type, bool display) {
	this->precipitationTypeVisibilities[type] = display;
	this->Modified();
}

void PrecipitationTypeFilter::enableUndefined(int enabled) {
	this->displayPrecipitationType(PrecipitationDataPoint::NONE, enabled);
}

void PrecipitationTypeFilter::enableRain(int enabled) {
	this->displayPrecipitationType(PrecipitationDataPoint::RAIN, enabled);
}

void PrecipitationTypeFilter::enableSnow(int enabled) {
	this->displayPrecipitationType(PrecipitationDataPoint::SNOW, enabled);
}

void PrecipitationTypeFilter::enableSleet(int enabled) {
	this->displayPrecipitationType(PrecipitationDataPoint::SLEET, enabled);
}

void PrecipitationTypeFilter::enableHail(int enabled) {
	this->displayPrecipitationType(PrecipitationDataPoint::HAIL, enabled);
}
