#ifndef KRONOS_TEMPERATURE_THRESHOLD_FILTER_HPP
#define KRONOS_TEMPERATURE_THRESHOLD_FILTER_HPP

#include <Filter/AbstractSelectionFilter.hpp>

#include <vtkAlgorithmOutput.h>

/**
 * This filter can extract data from temperature point sets read by a Kronos reader by limiting them to a certain temperature threshold.
 */
class TemperatureThresholdFilter : public AbstractSelectionFilter {
public:
	vtkTypeMacro(TemperatureThresholdFilter, vtkDataObjectAlgorithm);
	static TemperatureThresholdFilter* New();

	void SetInputConnection(vtkAlgorithmOutput* input);

	/**
	 * Callback for specifying the temperature threshold this filter uses.
	 * @param lowerLimit The lower temperature limit
	 * @param upperLimit The upper temperature limit
	 */
	void setThreshold(double lowerLimit, double upperLimit);

	/**
	 * Callback for the input array selection. This has to exist for the filter to be correctly assembled but can be ignored since the scalar is locked to the temperature values and the UI is hidden.
	 */
	void ignore(int id, int port, int connection, int fieldAssociation, const char* name) { }

private:
	/**
	 * Initialize a new instance of the temperature threshold filter.
	 */
	TemperatureThresholdFilter();
	~TemperatureThresholdFilter();

	TemperatureThresholdFilter(const TemperatureThresholdFilter&);  // Not implemented.
	void operator=(const TemperatureThresholdFilter&);  // Not implemented.

	QList<Data::Type> getCompatibleDataTypes();
	bool evaluatePoint(int pointIndex, Coordinate coordinate, vtkPointData* pointData);

	/**
	 * Stores the lower temperature limit for visible data points.
	 */
	double lowerLimit;

	/**
	 * Stores the upper temperature limit for visible data points.
	 */
	double upperLimit;
};

#endif
