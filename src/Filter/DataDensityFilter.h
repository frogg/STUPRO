#ifndef KRONOS_DATA_DENSITY_FILTER_HPP
#define KRONOS_DATA_DENSITY_FILTER_HPP

#include <Utils/Misc/Macros.hpp>

#include <vtkDataObjectAlgorithm.h>
#include <vtkPolyData.h>

class DataDensityFilter : public vtkDataObjectAlgorithm {

public:
	vtkTypeMacro(DataDensityFilter, vtkDataObjectAlgorithm)
	static DataDensityFilter* New();
	void PrintSelf(ostream& os, vtkIndent indent) override;

	/**
	 * Callback method for setting the percentage of points to keep.
	 * @param percentage The new percentage of points to keep
	 */
	void setDataPercentage(double percentage);
	void setDensityDistance(double distance);
	void setKMeansEnabled(bool enableKMeans);

protected:

	void KMeansDensityFilter(vtkPolyData* input, vtkPolyData* output);
	void NormalFilter(vtkPolyData* input, vtkPolyData* output);

	int RequestData(vtkInformation* info,
	                vtkInformationVector** inputVector,
	                vtkInformationVector* outputVector) override;


	int FillInputPortInformation(int port, vtkInformation* info) override;
	int FillOutputPortInformation(int port, vtkInformation* info) override;

private:
	/**
	 * Initiate a new data density filter.
	 */
	DataDensityFilter();
	~DataDensityFilter();

	DataDensityFilter(const DataDensityFilter&);  // Not implemented.
	void operator=(const DataDensityFilter&);  // Not implemented.

	/**
	 * The percentage of points that should be kept. 1 refers to keeping all points, 0 means keeping one point.
	 * May be pretty slow for values that are close to 1.
	 */
	double dataPercentage;

	double densityDistance;

	bool kMeansEnabled;
};

#endif
