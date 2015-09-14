#include "vtkGeoAlignedImageSource.h"


class LLGeoSource : public vtkGeoAlignedImageSource{
    int width, height;
public:
    //LLGeoSource (int, int);
    static int counter;
    LLGeoSource();
    static LLGeoSource* New()
    {return new LLGeoSource();}
    bool FetchChild(vtkGeoTreeNode* p, int index, vtkGeoTreeNode* c);
    bool FetchRoot(vtkGeoTreeNode* r);

private:
    class vtkProgressObserver;
    vtkProgressObserver* ProgressObserver;
};