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
#include <vtkInformationExecutivePortVectorKey.h>

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

int SphericalToCartesianFilter::ExecuteInformation(vtkInformation* request,
        vtkInformationVector** inputVector, vtkInformationVector* outputVector) {
	vtkInformation* info = outputVector->GetInformationObject(0);
	if (((GeometryTransform*) this->Transform)->getTransform()) {
		info->Set(Data::VTK_DATA_TRANSFORMATION(), Data::TRANSFORMED);
	} else {
		info->Set(Data::VTK_DATA_TRANSFORMATION(), Data::CONDENSED);
	}
	request->Append(vtkExecutive::KEYS_TO_COPY(), Data::VTK_DATA_TRANSFORMATION());

	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
	if (inInfo->Has(Data::VTK_DATA_TRANSFORMATION()) && firstRequestInformation) {
		Data::Transformation dataTransformation = static_cast<Data::Transformation>(inInfo->Get(
		            Data::VTK_DATA_TRANSFORMATION()));
		if (dataTransformation == Data::TRANSFORMED) {
			vtkErrorMacro( <<
			               "This filter transforms Data, but the input data seems to be already transformed");
		} else if (dataTransformation == Data::CONDENSED) {
			vtkErrorMacro( <<
			               "It seems that you already transformed the data, but did not click the transform-checkbox. "
			               << "Please activate the checkbox in the previous Spherical to Cartesian filter instead of applying another one.");
		}
	} else {
		KRONOS_LOG_INFO("The input data is no Kronos Data. Trying to use it anyway");
	}

	firstRequestInformation = false;
	return Superclass::ExecuteInformation(request, inputVector, outputVector);
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
