#ifndef DataPoint_h
#define DataPoint_h

#include <Globe/Coordinate.hpp>

/**
 * Holds a Coordinate along with its DataType.
 */
class DataPoint
{
public:
    /**
     * Constructs a DataPoint from DataType, Coordinate and Priority
     */
    DataPoint(int dataType, Coordinate coordinate, int priority);
    DataPoint() { }
    int getDataType() const;
    int getPriority() const;
    Coordinate getCoordinate() const;
    
    
private:
    Coordinate mCoordinate;
    int mDataType;
    int mPriority;
};


#endif /* DataPoint_h */
