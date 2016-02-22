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
#include <vtkSmartPointer.h>
#include <vtkPolyDataAlgorithm.h>

class TwitterHeatmapFilter : public vtkPolyDataAlgorithm {
    
public:
    vtkTypeMacro(TwitterHeatmapFilter, vtkPolyDataAlgorithm)
    static TwitterHeatmapFilter* New();
    void PrintSelf(ostream& os, vtkIndent indent) override;
    
    int toleranceValue = 0;
    void setToleranceValue(int newTolerance);
    
protected:
    
    int RequestData(vtkInformation* info,
                    vtkInformationVector** inputVector,
                    vtkInformationVector* outputVector) override;
    
    
    virtual int FillInputPortInformation(int port, vtkInformation* info) override;
    
    
private:
    TwitterHeatmapFilter();
    ~TwitterHeatmapFilter();
    
    
    TwitterHeatmapFilter(const TwitterHeatmapFilter&);  // Not implemented.
    void operator=(const TwitterHeatmapFilter&);  // Not implemented.
    
};



#endif /* TwitterHeatmapFilter_hpp */
