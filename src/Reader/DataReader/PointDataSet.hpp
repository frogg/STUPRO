#ifndef KRONOS_POINTDATASET_HPP
#define KRONOS_POINTDATASET_HPP

#include <QList>
#include <Reader/DataReader/DataPoints/DataPoint.hpp>

/**
* Stores a set of data points of one type in a list
*/
class PointDataSet {

public:
	/**
	 * Create a new PointDataSet which will store a set of points.
	 */
	PointDataSet();

	/**
	 * Add a new point of data to this set.
	 * @param dataPoint A pointer to the data point to be added
	 */
	void addPoint(DataPoint* dataPoint);

	/**
	 * Get a list of all data points in this set.
	 * @return A list of data point pointers this set stores
	 */
	QList<DataPoint*> getDataPoints();

private:
	QList<DataPoint*> points;

};

#endif