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
#include "GeometryTransform.h"

#include <cmath>

SphericalToCartesianFilter::SphericalToCartesianFilter() {
    this->transformGlobe = GeometryTransform::New(true);
    this->transformPassthrough = GeometryTransform::New(false);
    this->Transform = this->transformGlobe;
}

SphericalToCartesianFilter::~SphericalToCartesianFilter() {
    /*delete this->transformGlobe;
    delete this->transformPassthrough;*/
}

vtkStandardNewMacro(SphericalToCartesianFilter)

int SphericalToCartesianFilter::RequestData(vtkInformation *info,
                                            vtkInformationVector **inputVector,
                                            vtkInformationVector *outputVector) {
    
    this->Transform = this->transformGlobe;
    
    return this->Superclass::RequestData(info, inputVector, outputVector);
}

void SphericalToCartesianFilter::transformToCartesian(vtkPoints *points, double heightOffset) {
    // possible optimizations if necessary:
    //  * sin(x)^2 + cos(x)^2 = 1
    //  * use this method multi-threaded
    //  * use single precision
    
    // check for invalid input, bounds are returned as (xmin,xmax, ymin,ymax, zmin,zmax)
    double *bounds = points->GetBounds();
    if(bounds[0] < -180 || bounds[1] > 180 || bounds[2] < -90 || bounds[3] > 90 || bounds[4] < -heightOffset){
        vtkWarningMacro(<< "Data not in expected input scope: The data bounds:\n"
                        << "  Latitude:  (" << bounds[2] << "," << bounds[3] << ") -- expected: (-180,180)\n"
                        << "  Longitude: (" << bounds[0] << "," << bounds[1] << ") -- expected: ( -90, 90)\n"
                        << "  Height:    (" << bounds[4] << "," << bounds[5] << ") -- expected: (-" << heightOffset << ", beliebig")
    }

    for(int i=0; i<points->GetNumberOfPoints(); i++){
        double *point = points->GetPoint(i);

        //get longitude, latitude and radius out of points, heightOffset is a default value set in the header file
        double lon = point[0] * M_PI / 180;
        double lat = point[1] * M_PI / 180;
        double radius = heightOffset + point[2];

        //formula to transform the sphere coordinates to cartesian coordinate system
        //strange order because of ParaView cartesian coordinate system orientation
        point[2] = radius * cos(lat) * cos(lon);
        point[0] = radius * cos(lat) * sin(lon);
        point[1] = radius * sin(lat);

        points->SetPoint(i, point);
    }
}

void SphericalToCartesianFilter::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);
    
    os << indent << "Spherical to cartesian coordinate conversion, Kronos Project" << endl;
}

int SphericalToCartesianFilter::FillOutputPortInformation(int, vtkInformation* info) {
    // now add our info
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
