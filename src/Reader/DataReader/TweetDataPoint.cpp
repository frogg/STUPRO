#include "TweetDataPoint.hpp"

TweetDataPoint::TweetDataPoint(int dataType, Coordinate coordinate, int priority, int timestamp, QString author, QString content) :TemporalDataPoint(dataType, coordinate, priority, timestamp)
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