#include "SphericalToCartesianFilter.h"
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
#include "Reader/DataReader/Data.hpp"
#include "Utils/Misc/KronosLogger.hpp"

#include <cmath>

SphericalToCartesianFilter::SphericalToCartesianFilter() {
	//take this transformation, true -> it transforms per default
	this->Transform = GeometryTransform::New(true);
}

SphericalToCartesianFilter::~SphericalToCartesianFilter() {
}

vtkStandardNewMacro(SphericalToCartesianFilter)


void SphericalToCartesianFilter::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
	os << indent << "Spherical to cartesian coordinate conversion, Kronos Project" << endl;
}

int SphericalToCartesianFilter::RequestData(vtkInformation* info, vtkInformationVector** input,
        vtkInformationVector* output) {

	if (info->Get(Data::VTK_DATA_TRANSFORMATION()) == Data::TRANSFORMED) {
		KRONOS_LOG_ERROR("The data set is already transformed");
		return 1;
	}
	int retVal = Superclass::RequestData(info, input, output);

	if (((GeometryTransform*) this->Transform)->getTransform()) {
		output->GetInformationObject(0)->Set(Data::VTK_DATA_TRANSFORMATION(), Data::TRANSFORMED);
	} else {
		output->GetInformationObject(0)->Set(Data::VTK_DATA_TRANSFORMATION(), Data::CONDENSED);
	}

	return retVal;
}

int SphericalToCartesianFilter::FillOutputPortInformation(int, vtkInformation* info) {
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPointSet");
	return 1;
}

int SphericalToCartesianFilter::FillInputPortInformation(int, vtkInformation* info) {
	//make sure that we only accept vtkPointSet, vtkImageData and vtkRectilinearGrid as input
	info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkRectilinearGrid");
	return 1;
}

void SphericalToCartesianFilter::setTransform(bool value) {
	GeometryTransform* trans = ((GeometryTransform*) this->Transform);
	if (trans) {
		trans->setTransform(value);
	}
	//indicates that transformation should be done again
	this->Modified();
}
