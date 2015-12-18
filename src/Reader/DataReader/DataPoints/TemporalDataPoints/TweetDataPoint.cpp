#include "TweetDataPoint.hpp"

#include <Reader/DataReader/DataType.hpp>

TweetDataPoint::TweetDataPoint(Coordinate coordinate, int priority, int timestamp, QString author, QString content) :TemporalDataPoint(DataType::TWEETS, coordinate, priority, timestamp)
{
	this->author = author;
	this->content = content;
}

QString TweetDataPoint::getAuthor()
{
	return this->author;
};

QString TweetDataPoint::getContent()
{
	return this->content;
};