#ifndef KRONOS_TWEETDATAPOINT_HPP
#define KRONOS_TWEETDATAPOINT_HPP

#include <Reader/DataReader/DataPoints/TemporalDataPoints/TemporalDataPoint.hpp>
#include <QString>

/**
 * Holds a data point for a tweet with its author and content
 */
class TweetDataPoint : public TemporalDataPoint {

public:
	/**
	 * Create a new TweetDataPoint.
	 * @param coordinate The point's coordinates
	 * @param priority The point's zoom level priority
	 * @param timestamp The point's timestamp
	 * @param author The tweet's author
	 * @param content The tweet's content
	 * @param numberOfRetweets The number of retweets for this tweet
	 */
	TweetDataPoint(Coordinate coordinate, int priority, int timestamp, QString author,
	               QString content, int numberOfRetweets);

	/**
	 * Get the author of the Twitter message of this data point.
	 * @return The author of this point's tweet
	 */
	QString getAuthor() const;

	/**
	 * Get the content of the Twitter message of this data point.
	 * @return The content of this point's tweet
	 */
	QString getContent() const;

	/**
	 * Get the number of retweets for this tweet data point
	 * @return the number of retweets
	 */
	int getNumberOfRetweets() const;

private:
	QString author;
	QString content;
	int numberOfRetweets;
};

#endif