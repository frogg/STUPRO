#ifndef KRONOS_WIND_SPEED_THRESHOLD_FILTER_HPP
#define KRONOS_WIND_SPEED_THRESHOLD_FILTER_HPP

#include <Filter/AbstractSelectionFilter.hpp>

#include <vtkAlgorithmOutput.h>

/**
 * This filter can extract data from wind point sets read by a Kronos reader by limiting them to a certain wind speed threshold.
 */
class WindSpeedThresholdFilter : public AbstractSelectionFilter {
public:
	vtkTypeMacro(WindSpeedThresholdFilter, vtkDataObjectAlgorithm);
	static WindSpeedThresholdFilter* New();

	void SetInputConnection(vtkAlgorithmOutput* input);

	/**
	 * Callback for specifying the wind speed threshold this filter uses.
	 * @param lowerLimit The lower wind speed limit
	 * @param upperLimit The upper wind speed limit
	 */
	void setThreshold(double lowerLimit, double upperLimit);

	/**
	 * Callback for the input array selection. This has to exist for the filter to be correctly assembled but can be ignored since the scalar is locked to the wind values and the UI is hidden.
	 */
	void ignore(int id, int port, int connection, int fieldAssociation, const char* name) { }

private:
	/**
	 * Initialize a new instance of the wind speed threshold filter.
	 */
	WindSpeedThresholdFilter();
	~WindSpeedThresholdFilter();

	WindSpeedThresholdFilter(const WindSpeedThresholdFilter&);  // Not implemented.
	void operator=(const WindSpeedThresholdFilter&);  // Not implemented.

	QList<Data::Type> getCompatibleDataTypes();
	bool evaluatePoint(int pointIndex, Coordinate coordinate, vtkPointData* pointData);

	/**
	 * Stores the lower wind speed limit for visible data points.
	 */
	double lowerLimit;

	/**
	 * Stores the upper wind speed limit for visible data points.
	 */
	double upperLimit;
};

#endif
