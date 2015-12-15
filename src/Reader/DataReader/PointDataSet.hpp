#ifndef PointDataSet_H
#define PointDataSet_H

#include <QList>
#include "DataPoints/DataPoint.hpp"
/**
* stores all Datapoints of one type (e.g. Flight-Data) in one list 
*/
class PointDataSet
{
public:
	PointDataSet();
	/*
	* adds a DataPoint into a list, which contains only Data of a kind
	*/
	void addPoint(DataPoint* dataPoint);
	/*
	* @return The list containing severeral DataPoints, of a kind
	*/
	QList<DataPoint> getDataPoints();
private:
	QList<DataPoint> points;
};

#endif /* PointDataSet_H */