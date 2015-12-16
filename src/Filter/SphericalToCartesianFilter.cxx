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
    
    vtkPoints *points = output->GetPoints();
    for(int i=0; i<points->GetNumberOfPoints(); i++){
        points->SetPoint(i, transformToCartesian(points->GetPoint(i)));
    }
    
    return 1;
}

double* SphericalToCartesianFilter::transformToCartesian(double* point, double heightOffset)
{
    // possible optimizations:
    //  * sin(x)^2 + cos(x)^2 = 1
    //  * remove std::cout
    //  * use this method multi-threaded
    
    //invalid input
    if(point[0] > 180 || -180 > point[0] || point[1] > 90 || -90 > point[1]){
        vtkWarningMacro(<< "Latitude and Longitude not in expected input scope." << point[0] << ";" << point[1])
    }
    //get longitude, latitude and radius out of points, heightOffset is a default value set in the header file
    double lon = point[0] * M_PI / 180;
    double lat = point[1] * M_PI / 180;
    double radius = heightOffset + point[2];
    
    //formula to transform the sphere coordinates to cartesian coordinate system
    //strange order because of ParaView cartesian coordinate system orientation
    point[2] = radius * cos(lat) * cos(lon);
    point[0] = radius * cos(lat) * sin(lon);
    point[1] = radius * sin(lat);
    
    return point;
}

int SphericalToCartesianFilter::RequestUpdateExtent(vtkInformation *, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
    vtkDebugMacro("Request-Update");
    // get the info object
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    outInfo->Set(vtkPVInformationKeys::WHOLE_BOUNDING_BOX(),
                 -100,
                 100,
                 -100,
                 100,
                 -1000,
                 1000);
    
    /*
     int vtkStreamingDemandDrivenPipeline::SetWholeBoundingBox	( 	int 	port,
     double 	bb[6]
     )
     
     Set/Get the whole bounding box of an output port data object. The whole whole bounding box is meta data for data sets. It gets set by the algorithm during the update information pass.
     */
    return 1;
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
