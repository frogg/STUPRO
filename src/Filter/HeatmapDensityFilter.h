//
//  TwitterHeatmapFilter.hpp
//  kronos
//
//  Created by Frederik Riedel on 12.02.16.
//
//

#ifndef HeatmapDensityFilter_hpp
#define HeatmapDensityFilter_hpp

#include <stdio.h>
#include <Utils/Misc/Macros.hpp>
#include <vtkCleanUnstructuredGrid.h>
#include <vtkPointLocator.h>
#include <vtkTransformFilter.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataAlgorithm.h>

/**
 * Filter that receives vtkPolyData and calculates the data density for each point
 * the resolution can be changed dynamically
 */
class HeatmapDensityFilter : public vtkPolyDataAlgorithm {
    
public:
    vtkTypeMacro(HeatmapDensityFilter, vtkPolyDataAlgorithm)
    static HeatmapDensityFilter* New();
    void PrintSelf(ostream& os, vtkIndent indent) override;
    
    
    void setHeatmapResolution(double heatmapResolution);
    
protected:
    
    int RequestData(vtkInformation* info,
                    vtkInformationVector** inputVector,
                    vtkInformationVector* outputVector) override;
    
    
    virtual int FillInputPortInformation(int port, vtkInformation* info) override;
    virtual int FillOutputPortInformation(int port, vtkInformation* info) override;
    
private:
    HeatmapDensityFilter();
    ~HeatmapDensityFilter();
    
    
    HeatmapDensityFilter(const HeatmapDensityFilter&);  // Not implemented.
    void operator=(const HeatmapDensityFilter&);  // Not implemented.
    
    double heatmapResolution = 1.0;
};



#endif /* TwitterHeatmapFilter_hpp */
