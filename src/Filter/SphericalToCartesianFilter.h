#ifndef SphericalToCartesianFilter_hpp
#define SphericalToCartesianFilter_hpp

#include <Utils/Misc/Macros.hpp>
#include "vtkTransformFilter.h"
#include <vtkPoints.h>

/**
 * The SphericalToCartesianFilter class is a vtk filter which is able to convert the coordinate system of the data.
 * The input of a SphericalToCartesianFilter is a data set which uses gps data. These are converted to cartesian coordinates.
 */
class SphericalToCartesianFilter : public vtkTransformFilter {
    KRONOS_FRIEND_TEST(TestSphericalToCartesianFilter, ValidCalls);
public:
    vtkTypeMacro(SphericalToCartesianFilter, vtkPointSetAlgorithm)
    static SphericalToCartesianFilter *New();
    void PrintSelf(ostream& os, vtkIndent indent) override;

    int RequestData(vtkInformation *info,
                    vtkInformationVector **inputVector,
                    vtkInformationVector *outputVector) override;

    /**
     * Documentation see vtkAlgorithm
     */
    int FillOutputPortInformation(int, vtkInformation* info) override;
    
    /**
     * Documentation see vtkAlgorithm
     */
    int FillInputPortInformation(int, vtkInformation *info) override;

private:
    SphericalToCartesianFilter();
    ~SphericalToCartesianFilter();

    vtkAbstractTransform *transformGlobe = NULL;
    vtkAbstractTransform *transformPassthrough = NULL;

    SphericalToCartesianFilter(const SphericalToCartesianFilter&);  // Not implemented.
    void operator=(const SphericalToCartesianFilter&);  // Not implemented.
    
    /**
     * This method transforms the coordinate system of a point
     * @param points vtkPoints in gps coordinates. The output is written in place for speed reasons.
     * @param heightOffset the height offset to add to the height of the point
     * @warning outputs a warning if the input dataset is not in the correct bounds
     */
    void transformToCartesian(vtkPoints *points, double heightOffset = 100);
};

#endif /* SphericalToCartesianFilter_hpp */
