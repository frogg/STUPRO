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
	static TwitterFilter* New();

	void SetInputConnection(vtkAlgorithmOutput* input);

	/**
	  * Callback method for setting the authors whose tweets should be visible.
	  * @param authors Comma-separated string of all author names
	 */
	void setAuthors(const char* authors);

	/**
	 * Callback method for setting the keywords of visible tweets.
	 * @param keywords Comma-separated string of all keywords
	 */
	void setKeywords(const char* keywords);

	/**
	  * Callback method for setting the matching mode for authors
	  * @param mode (0 stands for containing, 1 for matching)
	 */
	void setAuthorMatchingMode(int mode);
    
    /**
     * Callback method for setting flight length range.
     * @param lowerLimit The lower flight length limit
     * @param upperLimit The upper flight length limit
     */
    void setRetweetThreshold(double lowerLimit, double upperLimit);
    
    /**
     * Callback for the input array selection. This has to exist for the filter to be correctly assembled but can be ignored since the scalar is locked to the temperature values and the UI is hidden.
     */
    void ignore(int id, int port, int connection, int fieldAssociation, const char* name) { }
    

private:
	/**
	 * Initialize a new instance of the Twitter filter.
	 */
	TwitterFilter();
	~TwitterFilter();

	TwitterFilter(const TwitterFilter&);  // Not implemented.
	void operator=(const TwitterFilter&);  // Not implemented.

	enum Mode {
		CONTAINING, MATCHING
	};

	/**
	 * The mode how author names are filtered.
	 */
	TwitterFilter::Mode authorMatchingMode;

	/**
	 * Check whether a tweet should be displayed based on its content.
	 * @param content The content of the tweet
	 * @return True if it should be displayed, false otherwise
	 */
	bool shouldDisplayBasedOnTweetContent(QString content);
	
	/**
	 * Check whether a tweet should be displayed based on its number of retweets.
	 * @param retweetNumber The number of retweets
	 * @return True if it should be displayed, false otherwise
	 */
    bool shouldDisplayBasedOnRetweets(int retweetNumber);

	QList<Data::Type> getCompatibleDataTypes();
	bool evaluatePoint(int pointIndex, Coordinate coordinate, vtkPointData* pointData);
    
	/**
	 * The lower limit of retweets whose tweets should still be displayed.
	 */
    double lowerRetweetLimit;
	
	/**
	 * The upper limit of retweets whose tweets should still be displayed.
	 */
    double upperRetweetLimit;

	/**
	 * Contains all author names whose tweets should be extracted.
	 */
	QStringList visibleAuthors;

	/**
	 * Contains all keywords of visible tweets.
	 */
	QStringList visibleKeywords;
};

#endif
