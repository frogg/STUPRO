#ifndef KRONOS_DATA_AGGREGATOR_HPP
#define KRONOS_DATA_AGGREGATOR_HPP

#include <Utils/Misc/PointCoordinates.hpp>
#include <Filter/TemporalAggregationFilter/AggregationValue.hpp>
#include <Reader/DataReader/Data.hpp>

#include <qmap.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>

/**
 * This class aggregates precipitation, temperature, wind and cloud coverage data and can return a poly data object as an output.
 */
class DataAggregator {

public:
	/**
	 * Create a new AggregationValue.
	 */
	DataAggregator();
	~DataAggregator();

	/**
	 * Define some attributes of the input data set that stay the same for all data points.
	 * @param dataType The type of the input data
	 * @param timeResolution The temporal resolution of the input data set
	 */
	void setDataSetAttributes(Data::Type dataType, int timeResolution);

	/**
	 * Add the data of a point to the aggregation.
	 * @param pointIndex The index of the current point in the data arrays
	 * @param coordinates The coordinates of the current point
	 * @param pointData A pointer to the point data in the current time step
	 */
	void addPointData(int pointIndex, PointCoordinates coordinates,
	                  vtkSmartPointer<vtkPointData> pointData);

	/**
	 * Generate a poly data object which contains all points that have been aggregated along with an array containing each point's aggregated value.
	 * @return A poly data object of the aggregated data
	 */
	vtkSmartPointer<vtkPolyData> getPolyData();

	/**
	 * Clear all aggregated values.
	 */
	void clearAggregationData();

private:
	/**
	 * Finish the aggregation of all data.
	 */
	void finishAggregation();

	/**
	 * This data structure identifies a point by its point coordinates and maps it to its aggregated data. The value will be a specialised subclass of AggregationValue depending on the type of input data. In each iteration over the time steps new data will be added to the aggregation.
	 */
	QMap<PointCoordinates, AggregationValue*> aggregatedData;

	/**
	 * Stores the data type of the input data.
	 */
	Data::Type dataType;

	/**
	 * Stores the time resolution of the input data.
	 */
	int timeResolution;

	/**
	 * Stores the highest time index this aggregator used.
	 */
	int lastTimeIndex;
};

#endif
