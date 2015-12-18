#ifndef TweetDataPoint_H
#define TweetDataPoint_H

#include "TemporalDataPoint.hpp"
#include <QString>
/**
* Holds a DataPoint for a Twitter-message at a specific Coordinate, which further includes its DataType, Coordinate, Priority, author and the content 
*/
class TweetDataPoint:TemporalDataPoint
{
public:
	TweetDataPoint(Coordinate coordinate, int priority, int timestamp, QString author, QString content);
	/**
	* @return The author of the Twitter-message
	*/
	QString getAuthor();
	/**
	* @return The content of the Twitter-message
	*/
	QString getContent();
private:
	QString author;
	QString content;
};

#endif /*TweetDataPoint_H*/


//warum #define QString int???