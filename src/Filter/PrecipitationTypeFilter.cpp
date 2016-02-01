#include <Filter/PrecipitationTypeFilter.h>

#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkCellIterator.h"
#include "vtkDataSet.h"
#include "vtkDemandDrivenPipeline.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkImageDataToPointSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkWarpScalar.h"
#include <vtkPoints.h>

#include <vtkPVInformationKeys.h>
#include <vtkAbstractTransform.h>
#include "GeometryTransform.hpp"

#include <cmath>

#include <iostream>

PrecipitationTypeFilter::PrecipitationTypeFilter() {
	// TODO
}

PrecipitationTypeFilter::~PrecipitationTypeFilter() { }

vtkStandardNewMacro(PrecipitationTypeFilter)

int PrecipitationTypeFilter::RequestData(vtkInformation* info,
        vtkInformationVector** inputVector,
        vtkInformationVector* outputVector) {
	return this->Superclass::RequestData(info, inputVector, outputVector);
}

void PrecipitationTypeFilter::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
	os << indent << "Precipitation data point extraction based on precipitation type, Kronos Project" << endl;
}

int PrecipitationTypeFilter::FillOutputPortInformation(int, vtkInformation* info) {
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPointSet");
	return 1;
}

int PrecipitationTypeFilter::FillInputPortInformation(int, vtkInformation* info) {
	info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
	return 1;
}

void PrecipitationTypeFilter::enableRain(int enabled) {
	std::cout << "Rain: " << enabled << std::endl;
}

void PrecipitationTypeFilter::enableSnow(int enabled) {
	std::cout << "Snow: " << enabled << std::endl;
}

void PrecipitationTypeFilter::enableSleet(int enabled) {
	std::cout << "Sleet: " << enabled << std::endl;
}

void PrecipitationTypeFilter::enableHail(int enabled) {
	std::cout << "Hail: " << enabled << std::endl;
}
