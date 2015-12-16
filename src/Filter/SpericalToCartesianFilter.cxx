#include "SpericalToCartesianFilter.h"

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

#include <cmath>

// Still the same everytime
vtkStandardNewMacro(SpericalToCartesianFilter)

int SpericalToCartesianFilter::RequestData(vtkInformation *,
                                            vtkInformationVector **inputVector,
                                            vtkInformationVector *outputVector) {
    vtkPointSet *output = this->createOutputData(vtkDataSet::GetData(inputVector[0]), outputVector);

    vtkPoints *points = output->GetPoints();
    //get the point of the output and transform them to cartesian coordinate system
    for(int i=0; i<points->GetNumberOfPoints(); i++){
        points->SetPoint(i, transformToCartesian(points->GetPoint(i)));
    }

    return 1;
}

double* SpericalToCartesianFilter::transformToCartesian(double* point,double heightOffset){
    double lon = point[0]*M_PI/180; //
    double lat = point[1]*M_PI/180; //
    double radius = heightOffset + point[2];

    std::cout << "Input: " << (int)point[0] << ";" << (int)point[1] << ";" << (int)point[2] << std::endl;

    point[0] = radius * cos(lat) * cos(lon);
    point[1] = radius * cos(lat) * sin(lon);
    point[2] = radius * sin(lat);

    std::cout << "Output: " << point[0] << ";" << point[1] << ";" << point[2] << std::endl;

    return point;
}

vtkPointSet *SpericalToCartesianFilter::createOutputData(vtkDataSet *const input, vtkInformationVector *outputVector) {
    vtkPointSet *output = vtkPointSet::GetData(outputVector);
    /*if (input->IsA("vtkPointSet")) {
        output->CopyStructure(vtkPointSet::SafeDownCast(input));
        output->CopyAttributes(vtkPointSet::SafeDownCast(input));
    } else {
      */  output = vtkStructuredGrid::New();
        outputVector->GetInformationObject(0)->Set(vtkDataObject::DATA_OBJECT(), output);

        // use a warp scalar filter to convert to a structured grid
        vtkSmartPointer<vtkWarpScalar> filter = vtkSmartPointer<vtkWarpScalar>::New();
        filter->SetInputData(input);
        filter->Update();
        vtkPointSet *filtered = filter->GetOutput();

        output->CopyStructure(filtered);
        output->CopyAttributes(filtered);
    //}

    return output;
}

int SpericalToCartesianFilter::ProcessRequest(vtkInformation *request,
                                              vtkInformationVector **inputVector,
                                              vtkInformationVector *outputVector) {
    
    if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA())) {
        return this->RequestData(request, inputVector, outputVector);
    }
    
    if(request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT())) {
        return 1;
    }
    
    // create the output
    if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT())) {
        return this->RequestDataObject(request, inputVector, outputVector);
    }
    
    // execute information
    if(request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION())) {
        return 1;
    }
    
    vtkWarningMacro(<< "ProcessRequest CALLED WITH NOTHING");
    return Superclass::ProcessRequest(request, inputVector, outputVector); //this->RequestData(request, inputVector, outputVector);
}

int SpericalToCartesianFilter::RequestDataObject(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector) {
    vtkImageData *inImage = vtkImageData::GetData(inputVector[0]);
    vtkPointSet *inPoints = vtkPointSet::GetData(inputVector[0]);
    vtkRectilinearGrid *inGrid = vtkRectilinearGrid::GetData(inputVector[0]);

    if (inImage || inGrid || inPoints) {
        vtkPointSet *output = vtkPointSet::GetData(outputVector);
        if (!output) {
            vtkNew<vtkStructuredGrid> newOutput;
            outputVector->GetInformationObject(0)->Set(vtkDataObject::DATA_OBJECT(), newOutput.GetPointer());
        }
        return 1;
    } else {
      return this->Superclass::RequestDataObject(request,
                                                 inputVector,
                                                 outputVector);
    }
}


void SpericalToCartesianFilter::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);
    
    os << indent << "Hello, this is our filter." << endl;
}

int SpericalToCartesianFilter::FillInputPortInformation(int port, vtkInformation *info) {
    info->Remove(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
    info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkRectilinearGrid");
    return 1;
}
