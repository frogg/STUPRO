#ifndef KRONOS_CITYDATAPOINT_HPP
#define KRONOS_CITYDATAPOINT_HPP

#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/NonTemporalDataPoint.hpp>
#include <QString>

/**
 * Holds a data point for a city with its name
 */
class CityDataPoint : public NonTemporalDataPoint {
    
public:
    /**
     * Create a new CityDataPoint.
     * @param coordinate The point's coordinates
     * @param priority The point's zoom level priority
     */
    CityDataPoint(Coordinate coordinate, int priority, QString name);
    
    /**
     * Get the name of the city this point describes.
     * @return The name of this point's city
     */
    QString getName() const;
    
private:
    QString name;

};

#endif
