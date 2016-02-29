#ifndef KRONOS_HEATMAP_DENSITY_FILTER_HPP
#define KRONOS_HEATMAP_DENSITY_FILTER_HPP

#include <Utils/Misc/Macros.hpp>

#include <vtkInformation.h>
#include <vtkPolyDataAlgorithm.h>

#include <qstring.h>

/**
 * Filter that receives vtkPolyData and calculates the data density for each point,
 * the resolution can be changed dynamically.
 */
class HeatmapDensityFilter : public vtkPolyDataAlgorithm {
    
public:
    vtkTypeMacro(HeatmapDensityFilter, vtkPolyDataAlgorithm)
    static HeatmapDensityFilter* New();
    void PrintSelf(ostream& os, vtkIndent indent) override;
    
    /**
     * Callback method for setting the horizontal grid resolution.
     * @param horizontalHeatmapResolution The new horizontal heatmap resolution
     */
    void setHorizontalHeatmapResolution(int horizontalHeatmapResolution);
    
    /**
     * Callback method for setting the vertical grid resolution.
     * @param verticalHeatmapResolution The new vertical heatmap resolution
     */
    void setVerticalHeatmapResolution(int verticalHeatmapResolution);
    
protected:
    int RequestData(vtkInformation* info,
                    vtkInformationVector** inputVector,
                    vtkInformationVector* outputVector) override;
    int RequestInformation(vtkInformation* request,
	                       vtkInformationVector** inputVector,
	                       vtkInformationVector* outputVector) override;
    virtual int FillInputPortInformation(int port, vtkInformation* info) override;
    virtual int FillOutputPortInformation(int port, vtkInformation* info) override;
    
    /**
     * Display an error message and remember that this filter does not hold valid data.
     * @param message The error message to be shown to the user
     */
    void fail(QString message);
    
private:
    /**
     * Initialise a new heatmap density filter.
     */
    HeatmapDensityFilter();
    ~HeatmapDensityFilter();
    
    HeatmapDensityFilter(const HeatmapDensityFilter&);  // Not implemented.
    void operator=(const HeatmapDensityFilter&);  // Not implemented.
    
    /**
     * This filter's horizontal grid resolution.
     */
    int horizontalResolution = 10;
    
    /**
     * This filter's vertical grid resolution.
     */
    int verticalResolution = 10;
    
    /**
	 * Boolean flag denoting whether there was an error.
	 */
	bool error;
};

#endif
