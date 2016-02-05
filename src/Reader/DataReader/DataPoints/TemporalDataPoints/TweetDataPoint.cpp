#include <Reader/DataReader/DataPoints/TemporalDataPoints/TweetDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

TweetDataPoint::TweetDataPoint(Coordinate coordinate, int priority, int timestamp, QString author,
<<<<<<< HEAD
                               QString content) : TemporalDataPoint(DataType::TWEETS, coordinate, priority, timestamp),
=======
                               QString content) : TemporalDataPoint(Data::TWEETS, coordinate, priority, timestamp),
>>>>>>> develop
	author(author), content(content) { }

QString TweetDataPoint::getAuthor() const {
	return this->author;
};

QString TweetDataPoint::getContent() const {
	return this->content;
};