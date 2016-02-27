#ifndef KRONOS_FLIGHT_FILTER_HPP
#define KRONOS_FLIGHT_FILTER_HPP

#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <iostream>

#include <Filter/AbstractSelectionFilter.hpp>
#include <QMap>
#include <QStringList>


/**
 * This filter can extract data from flight point sets read by a Kronos reader depending on the flight's origin and destination airport code, the airline operating the flight and its length.
 */
class FlightFilter : public AbstractSelectionFilter {
public:
	vtkTypeMacro(FlightFilter, vtkDataObjectAlgorithm)
	static FlightFilter* New();

	void SetInputConnection(vtkAlgorithmOutput* input);

	/**
	  * Callback method for setting the name of the airline(s) of the visible flights.
	  * @param airline name of the airline(s) whose flights should be visible. (comma-seperated)
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
	void setOriginAirportCode(const char* originAirportCode);

	/**
	 * Callback method for setting the airport codes of the destination airport.
	 * @param airport codes of the destination airports seperated by comma
	 */
	void setDestinationAirportCode(const char* destinationAirportCode);

	/**
	 * Callback method for setting flight length range.
	 * @param lowerLimit The lower flight length limit
	 * @param upperLimit The upper flight length limit
	 */
	void setFlightLengthThreshold(double lowerLimit, double upperLimit);

	/**
	 * Callback for the input array selection. This has to exist for the filter to be correctly assembled but can be ignored since the scalar is locked to the flight values and the UI is hidden.
	 */
	void ignore(int id, int port, int connection, int fieldAssociation, const char* name) { }


private:
	/**
	 * Initialize a new instance of the flight filter.
	 */
	FlightFilter();
	~FlightFilter();

	FlightFilter(const FlightFilter&);  // Not implemented.
	void operator=(const FlightFilter&);  // Not implemented.

	QList<Data::Type> getCompatibleDataTypes();
	bool evaluatePoint(int pointIndex, Coordinate coordinate, vtkPointData* pointData);

	/**
	 * check if data point is visible (based on airline filter)
	 * @param pointIndex index of the point whose visiblity is checked
	 * @param pointData contains all information about flight
	 * @return true, if data point is visible (based on airline filter)
	 */
	bool isVisibleBasedOnAirline(int pointIndex, vtkPointData* pointData);
	/**
	 * check if data point is visible (based on origin airport code filter)
	 * @param pointIndex index of the point whose visiblity is checked
	 * @param pointData contains all information about flight
	 * @return true, if data point is visible (based on origin airport code)
	 */
	bool isVisibleBasedOnOriginAirportCode(int pointIndex, vtkPointData* pointData);
	/**
	 * check if data point is visible (based on destination airport code filter)
	 * @param pointIndex index of the point whose visiblity is checked
	 * @param pointData contains all information about flight
	 * @return true, if data point is visible (based on destination airport code)
	 */
	bool isVisibleBasedOnDestinationAirportCode(int pointIndex, vtkPointData* pointData);
	/**
	 * check if data point is visible (based on flight length)
	 * @param pointIndex index of the point whose visiblity is checked
	 * @param pointData contains all information about flight
	 * @return true, if data point is visible (based on flight length)
	 */
	bool isVisibleBasedOnFlightLength(int pointIndex, vtkPointData* pointData);

	///enum Mode determines the string matching mode
	enum Mode {
		CONTAINING, MATCHING
	};

	/**
	 * extracts comma-seperated content and save it in string list
	 * @param inputString comma-seperated string
	 * @param list in which elements should be stored
	 */
	void updateStringList(QString inputString, QStringList& list);


	//determines the mode how airlines names are filtered
	FlightFilter::Mode airlineMatchingMode;
	//contains visible airlines names
	QStringList visibleAirlines;
	//contains visible airport codes of the origin airports
	QStringList visibleOriginAirportCodes;
	//contains visible airport codes of the destination airports
	QStringList visibleDestinationAirportCodes;
	//minimum flight length in km
	double minFlightLength;
	//maximum flight length in km
	double maxFlightLength;


};

#endif
