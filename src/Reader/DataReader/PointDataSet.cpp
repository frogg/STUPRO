#include "PointDataSet.hpp"

PointDataSet::PointDataSet()
{
	points = QList<DataPoint>();
}

void PointDataSet::addPoint(DataPoint* datapoint)
{
	if (datapoint != nullptr)
	{
		points.append(*datapoint);
	}
}

QList<DataPoint> PointDataSet::getDataPoints()
{
	return this->points;
}


