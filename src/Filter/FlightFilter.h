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
    void setAirlineMatchingMode(int mode);
    
    /**
     * Callback method for setting the airport codes of the origin airport.
     * @param airport codes of the origin airports seperated by comma
     */
    void setAirportCodeOrigin(const char* airportCodeOrigin);

    /**
     * Callback method for setting the airport codes of the destination airport.
     * @param airport codes of the destination airports seperated by comma
     */
    void setAirportCodeDestination(const char* airportCodeDestinatioin);

    /**
     * Callback method for setting miniumum flight length.
     * @param minimum flight length in km
     */
    void setMinFlightLength(double minLength);
    
    /**
     * Callback method for setting maximum flight length.
     * @param maximum flight length in km
     */
    void setMaxFlightLength(double maxLength);
    
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
    
    //returns true, if data point is visible (based on airline filter)
    bool visibleAirline(int pointIndex, vtkPointData* pointData);
    //returns true, if data point is visible (based on airport code origin)
    bool visibleAirportCodeOrigin(int pointIndex, vtkPointData* pointData);
    //returns true, if data point is visible (based on airport code destination)
    bool visibleAirportCodeDestination(int pointIndex, vtkPointData* pointData);
    //returns true, if data point is visible (based on flight length)
    bool visibleLength(int pointIndex, vtkPointData* pointData);
    
    enum Mode {
        CONTAINING, MATCHING
    };


    //determines the mode how airlines names are filtered
    FlightFilter::Mode modeAirline;
    //contains visible airlines names
    QStringList visibleAirlines;
    //contains visible airport codes of the origin airports
    QStringList visibleAirportCodesOrigin;
    //contains visible airport codes of the destination airports
    QStringList visibleAirportCodesDestination;
    //minimum flight length in km
    double minFlightLength;
    //maximum flight length in km
    double maxFlightLength;


};

#endif
