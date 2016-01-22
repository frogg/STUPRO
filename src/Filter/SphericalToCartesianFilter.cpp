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

#include <cmath>

SphericalToCartesianFilter::SphericalToCartesianFilter() {
    this->Transform = GeometryTransform::New(true);
}

SphericalToCartesianFilter::~SphericalToCartesianFilter() {
}

vtkStandardNewMacro(SphericalToCartesianFilter)

// TODO: Generate correct surface normals here or correct transformation

void SphericalToCartesianFilter::PrintSelf(ostream &os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
    os << indent << "Spherical to cartesian coordinate conversion, Kronos Project" << endl;
}

int SphericalToCartesianFilter::FillOutputPortInformation(int, vtkInformation *info) {
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPointSet");
	return 1;
}

int SphericalToCartesianFilter::FillInputPortInformation(int, vtkInformation *info) {
	info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
	info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkRectilinearGrid");
    return 1;
}

void SphericalToCartesianFilter::setTransform(bool value) {
    GeometryTransform *trans = ((GeometryTransform *) this->Transform);
    if (trans) {
        trans->setTransform(value);
    }
    this->Modified();
}
