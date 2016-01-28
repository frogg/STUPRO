#include <Reader/DataReader/DataPoints/TemporalDataPoints/TweetDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

TweetDataPoint::TweetDataPoint(Coordinate coordinate, int priority, int timestamp, QString author,
                               QString content) : TemporalDataPoint(Data::TWEETS, coordinate, priority, timestamp),
	author(author), content(content) { }

QString TweetDataPoint::getAuthor() const {
	return this->author;
};

QString TweetDataPoint::getContent() const {
	return this->content;
};