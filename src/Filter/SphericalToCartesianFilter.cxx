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

#include <vtkPVInformationKeys.h>

#include <cmath>

vtkStandardNewMacro(SphericalToCartesianFilter)

int SphericalToCartesianFilter::RequestData(vtkInformation *,
                                            vtkInformationVector **inputVector,
                                            vtkInformationVector *outputVector) {
    
    vtkPointSet *output = this->createOutputData(vtkDataSet::GetData(inputVector[0]), outputVector);
    
    transformToCartesian(output->GetPoints());
    
    return 1;
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

vtkPointSet *SphericalToCartesianFilter::createOutputData(vtkDataSet *const input, vtkInformationVector *outputVector) {
    vtkPointSet *output = vtkPointSet::GetData(outputVector);
    if (input && input->IsA("vtkPointSet") && output && output->IsA(input->GetClassName())) {
        output->CopyStructure(vtkPointSet::SafeDownCast(input));
        output->CopyAttributes(vtkPointSet::SafeDownCast(input));
    } else {
        output = vtkStructuredGrid::New();
        outputVector->GetInformationObject(0)->Set(vtkDataObject::DATA_OBJECT(), output);
        
        // use a warp scalar filter to convert to a structured grid
        vtkSmartPointer<vtkWarpScalar> filter = vtkSmartPointer<vtkWarpScalar>::New();
        filter->SetInputData(input);
        filter->Update();
        vtkPointSet *filtered = filter->GetOutput();
        
        output->CopyStructure(filtered);
        output->CopyAttributes(filtered);
    }
    return output;
}

int SphericalToCartesianFilter::ProcessRequest(vtkInformation *request,
                                               vtkInformationVector **inputVector,
                                               vtkInformationVector *outputVector) {
    
    if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA())) {
        return this->RequestData(request, inputVector, outputVector);
    } else if(request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT())) {
        return 1;
    } else if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT())) {
        return this->RequestDataObject(request, inputVector, outputVector);
    } else if(request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION())) {
        return 1;
    } else {
        return Superclass::ProcessRequest(request, inputVector, outputVector);
    }
}

int SphericalToCartesianFilter::RequestDataObject(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector) {
    vtkImageData *inImage = vtkImageData::GetData(inputVector[0]);
    vtkPointSet *inPoints = vtkPointSet::GetData(inputVector[0]);
    vtkRectilinearGrid *inGrid = vtkRectilinearGrid::GetData(inputVector[0]);
    
    vtkPointSet *output = vtkPointSet::GetData(outputVector);
    if ((inImage || inGrid) && !output) {
        vtkNew<vtkStructuredGrid> newOutput;
        outputVector->GetInformationObject(0)->Set(vtkDataObject::DATA_OBJECT(), newOutput.GetPointer());
    } else  if (inPoints && (!output || !output->IsA(inPoints->GetClassName()))) {
        output = inPoints->NewInstance();
        outputVector->GetInformationObject(0)->Set(vtkDataObject::DATA_OBJECT(), output);
        output->Delete();
    } else {
        return this->Superclass::RequestDataObject(request, inputVector, outputVector);
    }
    return 1;
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
