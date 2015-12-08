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

#include <math.h>

// Still the same everytime
vtkStandardNewMacro(SpericalToCartesianFilter);

int SpericalToCartesianFilter::RequestData(
                                            vtkInformation *vtkNotUsed(request),
                                            vtkInformationVector **inputVector,
                                            vtkInformationVector *outputVector) {
    
    // Read the info out of the vectors.
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    
    // Cast the input- and output-vectors to something useful.
    vtkDataSet *input = vtkDataSet::SafeDownCast(
                                                 inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkDataSet *output = vtkDataSet::SafeDownCast(
                                                  outInfo->Get(vtkDataObject::DATA_OBJECT()));
    
    // For now: use the exact input as output.
    output->CopyStructure(input);
    
    // TODO: Call different filters.
    
    vtkWarningMacro(<< " CALLED RequestData IN FLOW FILTER");
    
    return 1;
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