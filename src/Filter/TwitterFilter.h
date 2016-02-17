#ifndef KRONOS_TWITTER_FILTER_HPP
#define KRONOS_TWITTER_FILTER_HPP

#include <Filter/AbstractSelectionFilter.hpp>

#include <vtkPoints.h>
#include <vtkSmartPointer.h>

#include <qmap.h>
#include <qstringlist.h>

/**
 * This filter can extract data from Twitter point sets read by a Kronos reader depending on the tweet author and tweet content.
 */
class TwitterFilter : public AbstractSelectionFilter {
public:
    vtkTypeMacro(TwitterFilter, vtkDataObjectAlgorithm)
	static TwitterFilter *New();
    
    void SetInputConnection(vtkAlgorithmOutput *input);

    /**
      * Callback method for setting the authors whose tweets should be visible.
      * @param authors Comma-separated string of all author names
     */
    void setAuthors(const char* authors);
    
    /**
     * Callback method for setting the hashtags of visible tweets.
     * @param hashtags Comma-separated string of all hashtags
     */
    void setHashtags(const char* hashtags);

private:
    /**
     * Initialize a new instance of the Twitter filter.
     */
	TwitterFilter();
	~TwitterFilter();

	TwitterFilter(const TwitterFilter &); // Not implemented.
    void operator=(const TwitterFilter &); // Not implemented.
    
    QList<Data::Type> getCompatibleDataTypes();
    bool evaluatePoint(int pointIndex, Coordinate coordinate, vtkPointData* pointData);

    /**
     * Contains all author names whose tweets should be extracted.
     */
    QStringList authors;
    
    /**
     * Contains all hashtags of visible tweets.
     */
    QStringList hashtags;
};

#endif
