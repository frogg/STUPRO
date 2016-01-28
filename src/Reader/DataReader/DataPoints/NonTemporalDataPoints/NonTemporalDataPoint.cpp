#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/NonTemporalDataPoint.hpp>

NonTemporalDataPoint::NonTemporalDataPoint(int dataType, Coordinate coordinate, int priority) :
	DataPoint(dataType, coordinate, priority) { }