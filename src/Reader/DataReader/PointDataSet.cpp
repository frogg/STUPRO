#include "PointDataSet.hpp"

PointDataSet::PointDataSet()
{
	points = QList<DataPoint>();
}

void PointDataSet::addPoint(DataPoint& datapoint)
{
	points.append(datapoint);
}

QList<DataPoint> PointDataSet::getDataPoints()
{
	return this->points;
}


