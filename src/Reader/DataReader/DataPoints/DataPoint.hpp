#ifndef DataPoint_h
#define DataPoint_h



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
    int getDataType();
    int getPriority();
    Coordinate getCoordinate();
    
    
private:
    Coordinate mCoordinate;
    int mDataType;
    int mPriority;
};


#endif /* DataPoint_h */
