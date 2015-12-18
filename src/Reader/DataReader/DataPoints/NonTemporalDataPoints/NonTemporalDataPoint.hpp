#ifndef NonTemporalDataPoint_h
#define NonTemporalDataPoint_h


#include <Reader/DataReader/DataPoints/DataPoint.hpp>

/**
 * Holds a DataPoint for a Non-Temporal Datapoint
 * Data point is not time-sensitive and doesn't contain a timestamp
 * static data e.g. cities or available flight routes
 */
class NonTemporalDataPoint : public DataPoint {
public:
    
    //all parameters are passed to parent class
    //is there a more elegant way to do that?
    NonTemporalDataPoint(int dataType, Coordinate coordinate, int priority):DataPoint(dataType,coordinate,priority) {
    }
    
    
};



#endif /* NonTemporalDataPoint_h */
