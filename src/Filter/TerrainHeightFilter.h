#ifndef TerrainHeightFilter_hpp
#define TerrainHeightFilter_hpp

#include <Macros.hpp>
#include <vtkSetGet.h>
#include <vtkTransformFilter.h>
#include <iostream>

class TerrainHeightTransform;

/**
 * TerrainHeightFilter is a VTK filter that adjusts the Z coordinate of datasets to match the
 * earth's terrain surface.
 *
 * The input of a TerrainHeightFilter is a data set which uses gps data (latitude, longitude,
 * height). The output is data set in the same coordinate system, but with the height value
 * increased or decreased by the terrain height sample at that latitude/longitude.
 */
class TerrainHeightFilter: public vtkTransformFilter {
	KRONOS_FRIEND_TEST(TestTerrainHeightFilter, ValidCalls);

public:
	vtkTypeMacro(TerrainHeightFilter, vtkTransformFilter)
	static TerrainHeightFilter* New();
	void PrintSelf(ostream& os, vtkIndent indent) override;

	/**
	 * Documentation see vtkAlgorithm
	 */
	int FillOutputPortInformation(int, vtkInformation* info) override;

	/**
	 * Documentation see vtkAlgorithm
	 */
	int FillInputPortInformation(int, vtkInformation* info) override;

	/**
	 * Activates or deactivates heightmap clamping to 0. This prevents oceans from being sampled
	 * with a negative height value.
	 */
	void setClampingEnabled(bool clamp);

	/**
	 * Returns whether heightmap clamping to 0 is enabled for this filter.
	 */
	bool isClampingEnabled() const;

private:
	TerrainHeightFilter();
	~TerrainHeightFilter();

	TerrainHeightTransform* getTransform() const;

	TerrainHeightFilter(const TerrainHeightFilter&);  // Not implemented.
	void operator=(const TerrainHeightFilter&);  // Not implemented.
};

#endif /* SphericalToCartesianFilter_hpp */
