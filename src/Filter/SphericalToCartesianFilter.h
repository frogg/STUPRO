#ifndef SphericalToCartesianFilter_hpp
#define SphericalToCartesianFilter_hpp

#include <Utils/Misc/Macros.hpp>
#include "vtkPointSetAlgorithm.h"
#include <vtkSetGet.h>

class vtkPointSet;
class vtkDataSet;
class vtkPoints;

/**
 * The SphericalToCartesianFilter class is a vtk filter which is able to convert the coordinate system of the data.
 * The input of a SphericalToCartesianFilter is a data set which uses gps data. These are converted to cartesian coordinates.
 */
class SphericalToCartesianFilter : public vtkPointSetAlgorithm {
    KRONOS_FRIEND_TEST(TestSphericalToCartesianFilter, ValidCalls);
    KRONOS_FRIEND_TEST(TestSphericalToCartesianFilter, InvalidCalls);
    
public:
    vtkTypeMacro(SphericalToCartesianFilter, vtkPointSetAlgorithm)
    static SphericalToCartesianFilter *New();
    void PrintSelf(ostream& os, vtkIndent indent) override;
    
    /**
     * Documentation see vtkAlgorithm
     */
    int ProcessRequest(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector) override;
    
    /**
     * Documentation see vtkPointSetAlgorithm
     */
    int RequestDataObject(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector) override;
    
    /**
     * Documentation see vtkAlgorithm
     */
    int FillOutputPortInformation(int, vtkInformation* info) override;
    
    /**
     * Documentation see vtkAlgorithm
     */
    int FillInputPortInformation(int, vtkInformation *info) override;
protected:
    SphericalToCartesianFilter(){}
    ~SphericalToCartesianFilter(){}
    
    /**
     * Documentation see vtkPointSetAlgorithm
     */
    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

    int RequestUpdateExtent(vtkInformation *, vtkInformationVector **inputVector, vtkInformationVector *outputVector);
    
private:
    SphericalToCartesianFilter(const SphericalToCartesianFilter&);  // Not implemented.
    void operator=(const SphericalToCartesianFilter&);  // Not implemented.
    
    /**
     * This method transforms the coordinate system of a point
     * @param points vtkPoints in gps coordinates. The output is written in place for speed reasons.
     * @param heightOffset the height offset to add to the height of the point
     * @warning outputs a warning if the input dataset is not in the correct bounds
     */
    void transformToCartesian(vtkPoints *points, double heightOffset = 100);
    
    /**
     * Convenience method to copy the input to the output. Sometimes the output needs to be re-created and registered in Paraview.
     * @param input the input of the filter
     * @param outputVector the information about the output
     * @return a vtkPointSet registered as output of this filter
     */
    vtkPointSet *createOutputData(vtkDataSet* const input, vtkInformationVector *outputVector);

};

#endif /* SphericalToCartesianFilter_hpp */
