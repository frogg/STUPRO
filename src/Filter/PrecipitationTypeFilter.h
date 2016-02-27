#ifndef KRONOS_PRECIPITATION_TYPE_FILTER_HPP
#define KRONOS_PRECIPITATION_TYPE_FILTER_HPP

#include <qmap.h>
#include <qlist.h>

#include <Filter/AbstractSelectionFilter.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>

#include <vtkAlgorithmOutput.h>

/**
 * This filter can extract data from precipitation point sets read by a Kronos reader depending on the precipitation type of each point.
 */
class PrecipitationTypeFilter : public AbstractSelectionFilter {
public:
	vtkTypeMacro(PrecipitationTypeFilter, vtkDataObjectAlgorithm);
	static PrecipitationTypeFilter* New();

	void SetInputConnection(vtkAlgorithmOutput* input);

	/**
	 * Callback method for setting the visibility of precipitation of undefined type.
	 * @param enabled `1` if this precipitation type should be visible, `0` if it should be hidden
	 */
	void enableUndefined(int enabled);

	/**
	 * Callback method for setting the visibility of precipitation of type rain.
	 * @param enabled `1` if this precipitation type should be visible, `0` if it should be hidden
	 */
	void enableRain(int enabled);

	/**
	 * Callback method for setting the visibility of precipitation of type snow.
	 * @param enabled `1` if this precipitation type should be visible, `0` if it should be hidden
	 */
	void enableSnow(int enabled);

	/**
	 * Callback method for setting the visibility of precipitation of type sleet.
	 * @param enabled `1` if this precipitation type should be visible, `0` if it should be hidden
	 */
	void enableSleet(int enabled);

	/**
	 * Callback method for setting the visibility of precipitation of type hail.
	 * @param enabled `1` if this precipitation type should be visible, `0` if it should be hidden
	 */
	void enableHail(int enabled);

private:
	/**
	 * Initialize a new instance of the precipitation type filter.
	 */
	PrecipitationTypeFilter();
	~PrecipitationTypeFilter();

	PrecipitationTypeFilter(const PrecipitationTypeFilter&);  // Not implemented.
	void operator=(const PrecipitationTypeFilter&);  // Not implemented.

	QList<Data::Type> getCompatibleDataTypes();
	bool evaluatePoint(int pointIndex, Coordinate coordinate, vtkPointData* pointData);

	/**
	 * Internal method that handles VTK-related mechanics to set a specific precipitation type's visibility.
	 * @param type The precipitation type whose visibility should be changed
	 * @param display True if the precipitation type should be displayed, false otherwise
	 */
	void displayPrecipitationType(PrecipitationDataPoint::PrecipitationType type, bool display);

	/**
	 * An internal data structure that maps each precipitation type to a boolean value that denotes its visibility.
	 * This will be updated on callbacks and used to build necessary VTK structures.
	 */
	QMap<PrecipitationDataPoint::PrecipitationType, bool> precipitationTypeVisibilities;
};

#endif
