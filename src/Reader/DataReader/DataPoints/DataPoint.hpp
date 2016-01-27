#ifndef KRONOS_DATAPOINT_HPP
#define KRONOS_DATAPOINT_HPP

#include <Globe/Coordinate.hpp>
#include <Reader/DataReader/Data.hpp>

/**
 * Holds a coordinate along with the type of data this point contains
 */
class DataPoint {

public:
     /**
      * Create a new DataPoint.
      * @param dataType This point's data type
      * @param coordinate The point's coordinates
      * @param priority The point's zoom level priority
      */
    DataPoint(Data::Type dataType, Coordinate coordinate, int priority);
    virtual ~DataPoint() { }
    
    /**
     * Get the data type of this data point, referring to the constants defined in DataType.hpp.
     * @return The data type of this data point
     */
    Data::Type getDataType() const;
    
    /**
     * Get the minimum zoom level this data point should be displayed at.
     * @return The zoom level priority of this data point
     */
    int getPriority() const;
    
    /**
     * Get the coordinates of this data point.
     * @return The coordinates of this data point
     */
    Coordinate getCoordinate() const;
    
private:
    Coordinate coordinate;
    Data::Type dataType;
    int priority;

};

#endif
