#ifndef KRONOS_TWITTER_TYPE_FILTER_HPP
#define KRONOS_TWITTER_TYPE_FILTER_HPP

#include <vtkExtractSelection.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <iostream>


#include <QMap>

#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>

/**
 * This filter can extract data from precipitation point sets read by a Kronos reader depending on the precipitation type of each point.
 */
class TwitterFilter : public vtkExtractSelection {
public:
    vtkTypeMacro(TwitterFilter, vtkExtractSelection)
	static TwitterFilter *New();
	void PrintSelf(ostream &os, vtkIndent indent) override;

	int RequestData(vtkInformation *info,
					vtkInformationVector **inputVector,
					vtkInformationVector *outputVector) override;
    int RequestInformation(vtkInformation *request,
                    vtkInformationVector **inputVector,
                    vtkInformationVector *outputVector) override;
	int FillOutputPortInformation(int port, vtkInformation *info) override;
	int FillInputPortInformation(int port, vtkInformation *info) override;
	
//    /**
//     * Callback method for setting the visibility of precipitation of undefined type.
//     * @param enabled `1` if this precipitation type should be visible, `0` if it should be hidden
//     */
//	void enableUndefined(int enabled);
//    
    void setAuthorName(const char* authorName){
        std::cout << "test12345566" << authorName;
    }



private:
    /**
     * Initialize a new instance of the precipitation type filter.
     */
	TwitterFilter();
	~TwitterFilter();

	TwitterFilter(const TwitterFilter &); // Not implemented.
    void operator=(const TwitterFilter &); // Not implemented.
    
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
