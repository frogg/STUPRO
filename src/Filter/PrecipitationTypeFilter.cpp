#include <Filter/PrecipitationTypeFilter.h>

#include <Reader/DataReader/Data.hpp>

#include <vtkObjectFactory.h>
#include <vtkIntArray.h>

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
	vtkSmartPointer<vtkIntArray> precipitationTypeArray = vtkIntArray::SafeDownCast(
	            pointData->GetArray("precipitationTypes"));
				
	if (!precipitationTypeArray) {
		return false;
	}

	// For the point that should be checked, get its data type from the VTK data array and look up
	// in the QMap `precipitationTypeVisibilities` whether points with this data type should be
	// visible or not.
	return this->precipitationTypeVisibilities[static_cast<PrecipitationDataPoint::PrecipitationType>
	        (static_cast<int>(precipitationTypeArray->GetTuple1(pointIndex)))];
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
