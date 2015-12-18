#ifndef CityDataPoint_h
#define CityDataPoint_h



#include "NonTemporalDataPoint.hpp"
#include <QString>

/**
 * Holds a DataPoint for a city, which includes its DataType, Coordinate, Priority and Name
 */
class CityDataPoint:NonTemporalDataPoint {
    
public:
    CityDataPoint(Coordinate coordinate, int priority, QString name);
    QString getName() const;
    
private:
    QString mName;
    
};

#endif /* CityDataPoint_h */
