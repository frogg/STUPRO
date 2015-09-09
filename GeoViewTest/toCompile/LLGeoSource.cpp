// classes example
#include <iostream>
#include "LLGeoSource.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkProgressObserver.h"
#include "vtkGeoTreeNode.h"
#include "vtkGeoImageNode.h"

#include "vtkTexture.h"

using namespace std;

void LLGeoSource::set_values (int x, int y) {
    width = x;
    height = y;
}
/*
LLGeoSource::LLGeoSource (int a, int b) {
    width = a;
    height = b;
}
 */
LLGeoSource::LLGeoSource()
{
    this->Image = 0;
    this->LevelImages = vtkMultiBlockDataSet::New();
    this->LatitudeRange[0] = -90;
    this->LatitudeRange[1] = 90;
    this->LongitudeRange[0] = -180;
    this->LongitudeRange[1] = 180;
    //WARNING this has to be added
    //this->ProgressObserver = vtkProgressObserver::New();
    //this->ProgressObserver->SetTarget(this);
    this->PowerOfTwoSize = true;
    this->Overlap = 0.0;
    //counter = 0;
}
//methods to override
//FetchRoot(vtkGeoTreeNode* root) = 0;
//virtual bool FetchChild, FetchRoot


bool LLGeoSource::FetchChild(vtkGeoTreeNode* p, int index, vtkGeoTreeNode* c)
{
    //counter++;
    //cout << "called" + counter;
    cout << "test";
    vtkGeoImageNode* parent = 0;
    if (!(parent = vtkGeoImageNode::SafeDownCast(p)))
    {
        vtkErrorMacro(<< "Node must be an image node for this source.");
        return false;
    }
    vtkGeoImageNode* child = 0;
    if (!(child = vtkGeoImageNode::SafeDownCast(c)))
    {
        vtkErrorMacro(<< "Node must be an image node for this source.");
        return false;
    }
    int level = parent->GetLevel() + 1;
    unsigned int blockLevel = level + 1;
    if (blockLevel >= this->LevelImages->GetNumberOfBlocks())
    {
        vtkDebugMacro(<< "Reached max number of blocks (" << this->LevelImages->GetNumberOfBlocks() << ")");
        return false;
    }
    
    double lonRange[2];
    double latRange[2];
    double center[2];
    parent->GetLongitudeRange(lonRange);
    parent->GetLatitudeRange(latRange);
    center[0] = (lonRange[1] + lonRange[0])/2.0;
    center[1] = (latRange[1] + latRange[0])/2.0;
    
    child->SetLevel(level);
    if (index / 2)
    {
        child->SetLatitudeRange(center[1], latRange[1]);
    }
    else
    {
        child->SetLatitudeRange(latRange[0], center[1]);
    }
    if (index % 2)
    {
        child->SetLongitudeRange(center[0], lonRange[1]);
    }
    else
    {
        child->SetLongitudeRange(lonRange[0], center[0]);
    }
    
    int id = 0;
    if (level == 0)
    {
        // Special case: in the first level, the western hemisphere has id 0, and
        // the eastern hemisphere has id 1. This is to be compatible with the old
        // tile database format.
        if (index == 2)
        {
            id = 0;
        }
        else if (index == 3)
        {
            id = 1;
        }
        else if (index == 0)
        {
            vtkSmartPointer<vtkImageData> dummyImageWest = vtkSmartPointer<vtkImageData>::New();
            dummyImageWest->SetOrigin(-180.0, -270.0, 0.0);
            dummyImageWest->SetSpacing(0.0, -90.0, 0.0);
            child->GetTexture()->SetInputData(dummyImageWest);
            child->SetLatitudeRange(-270, -90);
            child->SetLongitudeRange(-180, 0);
            child->SetId(2);
            return true;
        }
        else if (index == 1)
        {
            vtkSmartPointer<vtkImageData> dummyImageEast = vtkSmartPointer<vtkImageData>::New();
            dummyImageEast->SetOrigin(0.0, -270.0, 0.0);
            dummyImageEast->SetSpacing(180.0, -90.0, 0.0);
            child->GetTexture()->SetInputData(dummyImageEast);
            child->SetLatitudeRange(-270, -90);
            child->SetLongitudeRange(0, 180);
            child->SetId(3);
            return true;
        }
    }
    else
    {
        id = parent->GetId() | (index << (2*level - 1));
    }
    child->SetId(id);
    
    // Crop and save the image.
    // Overwrite an image if it already exists.
    this->CropImageForNode(child, vtkImageData::SafeDownCast(this->LevelImages->GetBlock(blockLevel)));
    return true;
}


//anschauen: SetImage
/*
int main () {
    LLGeoSource rect (3,4);
    rect.set_values (3,4);
    cout << "area: " << rect.area();
    return 0;
}
*/