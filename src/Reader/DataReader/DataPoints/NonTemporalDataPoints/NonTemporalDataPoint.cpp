#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/NonTemporalDataPoint.hpp>

NonTemporalDataPoint::NonTemporalDataPoint(Data::Type dataType, Coordinate coordinate,
        int priority) : DataPoint(dataType, coordinate, priority) { }