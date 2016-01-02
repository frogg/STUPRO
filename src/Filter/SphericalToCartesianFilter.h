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
    vtkTypeMacro(SphericalToCartesianFilter, vtkTransformFilter)
	static SphericalToCartesianFilter *New();
	void PrintSelf(ostream &os, vtkIndent indent) override;

	int RequestData(vtkInformation *info,
					vtkInformationVector **inputVector,
					vtkInformationVector *outputVector) override;

	/**
	 * Documentation see vtkAlgorithm
	 */
	int FillOutputPortInformation(int, vtkInformation *info) override;

	/**
	 * Documentation see vtkAlgorithm
	 */
	int FillInputPortInformation(int, vtkInformation *info) override;

private:
	SphericalToCartesianFilter();
	~SphericalToCartesianFilter();

	vtkAbstractTransform *transformGlobe = NULL;
	vtkAbstractTransform *transformPassthrough = NULL;

	SphericalToCartesianFilter(const SphericalToCartesianFilter &); // Not implemented.
    void operator=(const SphericalToCartesianFilter &); // Not implemented.
};

#endif /* SphericalToCartesianFilter_hpp */
