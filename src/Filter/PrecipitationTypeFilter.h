#ifndef KRONOS_PRECIPITATION_TYPE_FILTER_HPP
#define KRONOS_PRECIPITATION_TYPE_FILTER_HPP

#include <vtkExtractSelection.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

#include <QMap>

#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>

/**
 * This filter can extract data from precipitation point sets read by a Kronos reader depending on the precipitation type of each point.
 */
class PrecipitationTypeFilter : public vtkExtractSelection {
public:
    vtkTypeMacro(PrecipitationTypeFilter, vtkExtractSelection)
	static PrecipitationTypeFilter *New();
	void PrintSelf(ostream &os, vtkIndent indent) override;

	int RequestData(vtkInformation *info,
					vtkInformationVector **inputVector,
					vtkInformationVector *outputVector) override;
    int RequestInformation(vtkInformation *request,
                    vtkInformationVector **inputVector,
                    vtkInformationVector *outputVector) override;
	int FillOutputPortInformation(int port, vtkInformation *info) override;
	int FillInputPortInformation(int port, vtkInformation *info) override;
	
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

	PrecipitationTypeFilter(const PrecipitationTypeFilter &); // Not implemented.
    void operator=(const PrecipitationTypeFilter &); // Not implemented.
    
    /**
     * Display an error message and remember that this filter does not hold valid data.
     * @param message The error message to be shown to the user
     */
    void fail(QString message);

    /**
     * Boolean flag denoting whether there was an error.
     */
    bool error;
    
    /**
     * Internal method that handles VTK-related mechanics to set a specific precipitation type's visibility.
     * @param type The precipitation type whose visibility should be changed
     * @param display True if the precipitation type should be displayed, false otherwise
     */
    void displayPrecipitationType(PrecipitationDataPoint::PrecipitationType type, bool display);
	
    /**
     * The `vtkSelection` that is being used internally to prune the data set.
     */
	vtkSmartPointer<vtkSelection> selection;
	
    /**
     * An internal data structure that maps each precipitation type to a boolean value that denotes its visibility.
     * This will be updated on callbacks and used to build necessary VTK structures.
     */
	QMap<PrecipitationDataPoint::PrecipitationType, bool> precipitationTypeVisibilities;
};

#endif
