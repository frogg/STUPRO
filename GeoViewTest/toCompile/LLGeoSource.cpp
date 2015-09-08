// classes example
#include <iostream>
#include "LLGeoSource.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkProgressObserver.h"

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
}

/*
int main () {
    LLGeoSource rect (3,4);
    rect.set_values (3,4);
    cout << "area: " << rect.area();
    return 0;
}
*/