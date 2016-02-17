#ifndef KRONOS_FLIGHT_FILTER_HPP
#define KRONOS_FLIGHT_FILTER_HPP

#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <iostream>

#include <Filter/AbstractSelectionFilter.hpp>
#include <QMap>

/**
 * This filter can extract data from flight point sets read by a Kronos reader depending on the start, end flight route, airline and maximum flights.
 */
class FlightFilter : public AbstractSelectionFilter {
public:
    vtkTypeMacro(FlightFilter, vtkDataObjectAlgorithm)
	static FlightFilter *New();
    
    void SetInputConnection(vtkAlgorithmOutput *input);

    /**
      * Callback method for setting the name of the airline(s) of the visible flights.
      * @param airline
     */
    void setAirline(const char* airline);
    
    /**
     * Callback method for setting the matching mode
     * @param mode (0 stands for containing, 1 for matching)
     */
    void setMatchingMode(int mode);
    

private:
    /**
     * Initialize a new instance of the flight filter.
     */
	FlightFilter();
	~FlightFilter();

	FlightFilter(const FlightFilter &); // Not implemented.
    void operator=(const FlightFilter &); // Not implemented.
    
    QList<Data::Type> getCompatibleDataTypes();
    bool evaluatePoint(int pointIndex, Coordinate coordinate, vtkPointData* pointData);
	
    enum Mode {
        CONTAINING, MATCHING
    };


    //determines the mode how airlines names are filtered
    FlightFilter::Mode modeAirline;
    //contains visible airlines names
    QStringList visibleAirlines;


};

#endif
