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


/////////////////////////////////////////////////////////////////////////////////////////
// vtkKronosReader

vtkStandardNewMacro(vtkKronosReader);


vtkKronosReader::vtkKronosReader() : cameraPos(),globeRadius(0.0),distanceToFocalPoint(0.0),fileName(""),zoomLevel(0)
{

    if(Configuration::getInstance().hasKey("globe.radius")){
        globeRadius = Configuration::getInstance().getDouble("globe.radius");
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

int vtkKronosReader::RequestData(
  vtkInformation*,
  vtkInformationVector**,
  vtkInformationVector* outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
    
    cout << "ZOOMLEVEL " << zoomLevel << endl;
    
    JsonReader jsonReader = JsonReaderFactory::createReader(this->fileName);
    
    vtkSmartPointer<vtkPolyData> polyData = jsonReader.getVtkDataSet(this->zoomLevel);
    output->DeepCopy(polyData);

  return 1;
}
