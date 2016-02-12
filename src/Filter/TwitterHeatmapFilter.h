//
//  TwitterHeatmapFilter.hpp
//  kronos
//
//  Created by Frederik Riedel on 12.02.16.
//
//

#ifndef TwitterHeatmapFilter_hpp
#define TwitterHeatmapFilter_hpp

#include <stdio.h>
#include <Utils/Misc/Macros.hpp>
#include <vtkCleanUnstructuredGrid.h>
#include <vtkPointLocator.h>
#include <vtkTransformFilter.h>

class TwitterHeatmapFilter : public vtkTransformFilter {

public:
    vtkTypeMacro(TwitterHeatmapFilter, vtkTransformFilter)
    static TwitterHeatmapFilter *New();
    void PrintSelf(ostream &os, vtkIndent indent) override;
    
    
protected:
    vtkPointLocator *Locator;
    
    int RequestData(vtkInformation *info,
                    vtkInformationVector **inputVector,
                    vtkInformationVector *outputVector) override;

    
    virtual int FillInputPortInformation(int port, vtkInformation *info) override;
    
    
private:
    TwitterHeatmapFilter();
    ~TwitterHeatmapFilter();
    
    
    TwitterHeatmapFilter(const TwitterHeatmapFilter &); // Not implemented.
    void operator=(const TwitterHeatmapFilter &); // Not implemented.

};



#endif /* TwitterHeatmapFilter_hpp */
