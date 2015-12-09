/**
 */
#include "SpericalToCartesianFilter.h"

#include "vtkCellData.h"
#include "vtkDataSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkDemandDrivenPipeline.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"


#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkCellIterator.h"

#include <cmath>

// Still the same everytime
vtkStandardNewMacro(SpericalToCartesianFilter)

int SpericalToCartesianFilter::RequestData(vtkInformation *vtkNotUsed(request),
                                            vtkInformationVector **inputVector,
                                            vtkInformationVector *outputVector) {
    
    // Read the info out of the vectors.
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    
    // Cast the input- and output-vectors to something useful.
    vtkDataSet *input = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
    
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
    // For now: use the exact input as output.
    output->CopyStructure(input);
    output->CopyAttributes(input);

    vtkPoints *points = output->GetPoints();
    for(int i=0; i<points->GetNumberOfPoints(); i++){
        points->SetPoint(i, transformToCartesian(points->GetPoint(i)));
    }

    return 1;
}

double* SpericalToCartesianFilter::transformToCartesian(double* point,double heightOffset){
    double lat = point[0]*M_PI/180 + M_PI/2;
    double longi = point[1]*M_PI/180 + M_PI;
    double radius = heightOffset + point[2];
    point[0] = radius * sin(lat) * cos(longi);
    point[1] = radius * sin(lat) * sin(longi);
    point[2] = radius * cos(lat);
    std::cout <<  point[0] << ";" << point[1] << ";" << point[2] << std::endl;
    return point;
}

int SpericalToCartesianFilter::ProcessRequest(
                                               vtkInformation *request,
                                               vtkInformationVector **inputVector,
                                               vtkInformationVector *outputVector) {
    
    if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA())) {
        vtkWarningMacro(<< "ProcessRequest CALLED WITH REQUEST_DATA");
        return this->RequestData(request, inputVector, outputVector);
    }
    
    if(request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT())) {
        vtkWarningMacro(<< "ProcessRequest CALLED WITH REQUEST_UPDATE_EXTENT");
        return this->RequestUpdateExtent(request, inputVector, outputVector);
    }
    
    // create the output
    if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT())) {
        vtkDebugMacro(<< "ProcessRequest CALLED WITH REQUEST_DATA_OBJECT");
        return this->RequestDataObject(request, inputVector, outputVector);
    }
    
    // execute information
    if(request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION())) {
        vtkWarningMacro(<< "ProcessRequest CALLED WITH REQUEST_INFORMATION");
        return this->RequestInformation(request, inputVector, outputVector);
    }
    
    vtkWarningMacro(<< "ProcessRequest CALLED WITH NOTHING");
    return Superclass::ProcessRequest(request, inputVector, outputVector); //this->RequestData(request, inputVector, outputVector);
}


void SpericalToCartesianFilter::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);
    
    os << indent << "Hello, this is our filter." << endl;
}
