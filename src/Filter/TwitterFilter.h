#ifndef KRONOS_TWITTER_TYPE_FILTER_HPP
#define KRONOS_TWITTER_TYPE_FILTER_HPP

#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <iostream>

#include <Filter/AbstractSelectionFilter.hpp>
#include <QMap>

/**
 * This filter can extract data from twitter point sets read by a Kronos reader depending on the twitter type of each point.
 */
class TwitterFilter : public AbstractSelectionFilter {
public:
    vtkTypeMacro(TwitterFilter, vtkDataObjectAlgorithm)
	static TwitterFilter *New();
    
    void SetInputConnection(vtkAlgorithmOutput *input);

    /**
      * Callback method for setting the name of the author(s) of the visible tweets.
      * @param authorName
     */
    void setAuthorName(const char* authorName);
    
    /**
     * Callback method for setting the matching mode
     * @param mode (0 stands for containing, 1 for matching)
     */
    void setMatchingMode(int mode);
    

private:
    /**
     * Initialize a new instance of the twitter filter.
     */
	TwitterFilter();
	~TwitterFilter();

	TwitterFilter(const TwitterFilter &); // Not implemented.
    void operator=(const TwitterFilter &); // Not implemented.
    
    QList<Data::Type> getCompatibleDataTypes();
    bool evaluatePoint(int pointIndex, Coordinate coordinate, vtkPointData* pointData);
	
    QList<QString> visibleAuthorName;
    
    /**
     * An internal data structure that maps each precipitation type to a boolean value that denotes its visibility.
     * This will be updated on callbacks and used to build necessary VTK structures.
     */
	//QMap<PrecipitationDataPoint::PrecipitationType, bool> precipitationTypeVisibilities;
    //need a QList with authors, whose tweets should be printed
};

#endif
