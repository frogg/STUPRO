#include "vtkGeoAlignedImageSource.h"


class LLGeoSource : public vtkGeoAlignedImageSource{
    int width, height;
public:
    void set_values (int,int);
    int area() {return width*height;}
    //LLGeoSource (int, int);
    LLGeoSource();
    static LLGeoSource* New()
    {return new LLGeoSource();}
};