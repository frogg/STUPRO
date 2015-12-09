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
#include "vtkPolyData.h"


#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"

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
  //  vtkDataSet *output = vtkDataSet::SafeDownCast(
   //                                               outInfo->Get(vtkDataObject::DATA_OBJECT()));
    
    vtkPolyData *output = vtkPolyData::SafeDownCast(
                                                    outInfo->Get(vtkDataObject::DATA_OBJECT()));
    // For now: use the exact input as output.
    
    double coodinate[3];
    for(int i=0; i<input->GetNumberOfPoints(); i++){
        input->GetPoint(i,coodinate);//->SetData;
        vtkWarningMacro(<< coodinate[0] << ";" << coodinate[1] << ";" << coodinate[2]  << "number of points"
                        );
        //coodinate[0] = 1000; //doesn't work
    }
    //points defining a cell
    vtkWarningMacro(<< input->GetNumberOfCells()  << "number of cells");

//    output->CopyStructure(input);
    
    int nmbPoints=4;
    
    vtkCellArray *newPolys;
    newPolys = vtkCellArray::New();
    newPolys->Allocate(newPolys->EstimateSize(nmbPoints, 3));
    
    vtkPoints *newPoints;
    newPoints = vtkPoints::New();
    vtkFloatArray *newNormals;
    newPoints->Allocate(nmbPoints);
    newNormals = vtkFloatArray::New();
    newNormals->SetNumberOfComponents(3);
    newNormals->Allocate(3*nmbPoints);
    newNormals->SetName("Normals");
    vtkIdType pts[4];
    
    for(int i=0; i<input->GetNumberOfPoints()/3; i++){
        input->GetPoint(i,coodinate);
        pts[0]  = newPoints->InsertNextPoint(coodinate[0]*2,coodinate[1]*2,coodinate[2]);
        input->GetPoint(i+1,coodinate);
        pts[1]  = newPoints->InsertNextPoint(coodinate[0]*2,coodinate[1]*2,coodinate[2]);
        input->GetPoint(i+2,coodinate);
        pts[2]  = newPoints->InsertNextPoint(coodinate[0]*2,coodinate[1]*2,coodinate[2]);
        newPolys->InsertNextCell(3, pts);
    }
 
    

    
    // output->CopyStructure( input );
    newPoints->Squeeze();
    output->SetPoints(newPoints);
    newPoints->Delete();
    
 /*   newNormals->Squeeze();
    output->GetPointData()->SetNormals(newNormals);
    newNormals->Delete();
*/
    newPolys->Squeeze();
    output->SetPolys(newPolys);
    newPolys->Delete();

    /*--test */
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