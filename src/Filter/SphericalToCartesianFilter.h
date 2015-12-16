#ifndef SphericalToCartesianFilter_hpp
#define SphericalToCartesianFilter_hpp

#include "vtkPointSetAlgorithm.h"

class vtkPointSet;
class vtkDataSet;

/**
 * The SphericalToCartesianFilter class is a vtk filter which is able to convert the coordinate system of the data.
 * The input of a SphericalToCartesianFilter is a data set which uses gps data. These are converted to cartesian coordinates.
 */
class SphericalToCartesianFilter : public vtkPointSetAlgorithm {
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

private:
    SphericalToCartesianFilter(const SphericalToCartesianFilter&);  // Not implemented.
    void operator=(const SphericalToCartesianFilter&);  // Not implemented.

    /**
     * This method transforms the coordinate system of a point
     * @param point a point in gps coordinates. Note that the output is written directly into this array for speed reasons.
     * @param heightOffset the height offset to add to the height of the point
     * @return the resulting point (this is the same as point)
     */
    double* transformToCartesian(double* point,double heightOffset = 100);

    /**
     * Convenience method to copy the input to the output. Sometimes the output needs to be re-created and registered in Paraview.
     * @param input the input of the filter
     * @param outputVector the information about the output
     * @return a vtkPointSet registered as output of this filter
     */
    vtkPointSet *createOutputData(vtkDataSet* const input, vtkInformationVector *outputVector);
};

#endif /* SphericalToCartesianFilter_hpp */
