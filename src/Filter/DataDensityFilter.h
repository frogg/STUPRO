#ifndef KRONOS_DATA_DENSITY_FILTER_HPP
#define KRONOS_DATA_DENSITY_FILTER_HPP

#include <Utils/Misc/Macros.hpp>

#include <vtkDataObjectAlgorithm.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <qmap.h>
#include <qlist.h>
#include <Utils/Misc/PointCoordinates.hpp>

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
	
	/**
	 * Callback method for setting whether the kMeans algorithm should be used.
	 * @param enableKMeans True if kMeans should be used, false otherwise
	 */
	void setKMeansEnabled(bool enableKMeans);

protected:
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
	 * Reduce a set of points using the kMeans algorithm.
	 * @param input The input point set
	 * @return A map of point coordinates of central points and a list of point IDs that belong to them
	 */
	QMap<PointCoordinates, QList<int>> reducePointsKMeans(vtkPointSet* input);
	
	/**
	 * Reduce a set of points using a simple approach.
	 * @param input The input point set
	 * @return A map of point coordinates of central points and a list of point IDs that belong to them
	 */
	QMap<PointCoordinates, QList<int>> reducePointsSimple(vtkPointSet* input);
	
	/**
	 * Output poly data from a map of central point coordinates, their associated subordinate points and all point data.
	 * @param centralPoints A map of point coordinates of central points and a list of point IDs that belong to them
	 * @param input The input point set
	 * @return A poly data object with all central points and averaged scalar values
	 */
	vtkSmartPointer<vtkPolyData> generateOutputData(QMap<PointCoordinates, QList<int>> centralPoints, vtkPointSet* input);

	/**
	 * The percentage of points that should be kept. 1 refers to keeping all points, 0 means keeping one point.
	 * May be pretty slow for values that are close to 1.
	 */
	double dataPercentage;

	/**
	 * Boolean flag denoting whether the kMeans algorithm should be used.
	 */
	bool kMeansEnabled;
};

#endif
