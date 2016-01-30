#include "vtkKronosReader.h"
#include "vtkCommand.h"
#include "vtkDataSetAttributes.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTable.h"
#include "vtkUnicodeStringArray.h"
#include "vtkStringArray.h"
#include "vtkStringToNumeric.h"

#include "vtkTextCodec.h"
#include "vtkTextCodecFactory.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <set>
#include <vector>
#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"
#include "vtkDoubleArray.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkGenericCell.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkTensor.h"
#include "vtkPolyData.h"
#include <vtkCellArray.h>
#include "vtkPointData.h"
#include "vtkPoints.h"
#include <math.h>
#include <ctype.h>
#include <cmath>
#include <Reader/DataReader/JsonReaderFactory.hpp>
#include <Reader/DataReader/JsonReader.hpp>
#include <Utils/Config/Configuration.hpp>

#include <thread>


/////////////////////////////////////////////////////////////////////////////////////////
// vtkKronosReader

vtkStandardNewMacro(vtkKronosReader);


vtkKronosReader::vtkKronosReader() : cameraPos(),globeRadius(0.0),distanceToFocalPoint(0.0),fileName(""),zoomLevel(0),error(false)
{
    this->SetNumberOfInputPorts(0);
    this->SetNumberOfOutputPorts(1);
    if(Configuration::getInstance().hasKey("globe.radius")){
        globeRadius = Configuration::getInstance().getDouble("globe.radius");
    } else {
        this->error = true;
    }
    this->SetNumberOfInputPorts(0);
    this->SetNumberOfOutputPorts(1);

}

vtkKronosReader::~vtkKronosReader()
{
    
}



void vtkKronosReader::SetFileName(std::string name){
    //Set Filename
    this->fileName=QString::fromStdString(name);
    
    try {
        this->jsonReader = JsonReaderFactory::createReader(this->fileName);
    } catch (const ReaderException& e) {
        vtkErrorMacro(<< e.what());
        this->error = true;
        return;
    }
    
    std::thread cacheThread(&JsonReader::cacheAllData, this->jsonReader.get());
    cacheThread.detach();
}



void vtkKronosReader::SetCameraPos(double x,double y,double z){
    this->cameraPos = Vector3d(x,y,z);
    //Calcs distanz to 0,0,0
    this->distanceToFocalPoint = (this->cameraPos).length();
    //Check if Zoomlevel Changed
    int tempZoom = this->zoomLevel;
    calcLOD();
    if (tempZoom!=this->zoomLevel) {
        //Set Modified flag;
        this->Modified();
    }
}


void vtkKronosReader::calcLOD(){
    int LOD = 0;
    if(this->globeRadius!=0.0f){
        float distanceToSurface=this->distanceToFocalPoint-this->globeRadius;
        if (distanceToSurface>0.0f){
            int count = 10;
            for(int i = 0 ; i < 10; i++){
                count-=1;
                if (this->globeRadius*(i+1) > distanceToSurface) {
                    LOD=count;
                    break;
                }
            }
        }
        else{
            //if camera is in GLOBUS (normaly not the case)
            LOD=0;
        }
    }
    this->zoomLevel=int(LOD);
}

int vtkKronosReader::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector,
        vtkInformationVector *outputVector) {
    if (this->error) {
        return 1;
    }
            
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // Add information to the output vector if the data contains time information
    if (this->jsonReader->hasTemporalData()) {
        double timeStepValue = this->jsonReader->getTimeStepSize();
        outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), &timeStepValue, 1);
        
        double timeRange[2];
        timeRange[0] = 0;
        timeRange[1] = 1;
        outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), timeRange, 2);
    }

    return 1;
}

int vtkKronosReader::RequestData(vtkInformation*, vtkInformationVector**,
        vtkInformationVector* outputVector) {
    if (this->error) {
        return 1;
    }
    
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkSmartPointer<vtkPolyData> output = vtkPolyData::SafeDownCast(
        outInfo->Get(vtkDataObject::DATA_OBJECT())
    );
    
    if (this->jsonReader != nullptr) {
        if (outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP()) &&
                this->jsonReader->hasTemporalData()) {
            double requestedTimeValue = outInfo->Get(
                vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP()
            );
            
            vtkSmartPointer<vtkPolyData> polyData = this->jsonReader->getVtkDataSet(
                this->zoomLevel,
                requestedTimeValue
            );
            
            output->DeepCopy(polyData);
        } else {
            vtkSmartPointer<vtkPolyData> polyData = this->jsonReader->getVtkDataSet(
                this->zoomLevel
            );
            output->DeepCopy(polyData);
        }
    }
    
    return 1;
}
