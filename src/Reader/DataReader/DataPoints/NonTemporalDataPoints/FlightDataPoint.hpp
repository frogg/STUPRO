#ifndef FlightDataPoint_h
#define FlightDataPoint_h

#include "NonTemporalDataPoint.hpp"


/**
 * Holds information for a flight (departure location and destination), its dataType and its priority
 */
class FlightDataPoint : public NonTemporalDataPoint {
    
public:
    FlightDataPoint(Coordinate coordinate, int priority, Coordinate destination);
    Coordinate getDestination() const;
    
private:
    Coordinate mDestination;
    
};

#endif /* FlightDataPoint_h */
