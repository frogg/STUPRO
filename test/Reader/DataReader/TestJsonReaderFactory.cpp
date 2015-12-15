#include <gtest/gtest.h>

#include <string>

#include <Reader/DataReader/JsonReaderFactory.hpp>
#include <Reader/DataReader/AbstractJsonReader.hpp>
#include <Reader/DataReader/DataType.hpp>

TEST(TestJsonReaderFactory, ReadValidFiles) {
	// Test reading some city data
	AbstractJsonReader cityReader = JsonReaderFactory::createReader("res/test-data/cities.json");

	EXPECT_EQ(
		cityReader.getDataType(),
		DataType::CITIES
	);
	
	EXPECT_EQ(
		cityReader.hasTemporalData(),
		false
	);
	
	// Test reading some tweets
	AbstractJsonReader tweetReader = JsonReaderFactory::createReader("res/test-data/tweets.json");

	EXPECT_EQ(
		tweetReader.getDataType(),
		DataType::TWEETS
	);
	
	EXPECT_EQ(
		tweetReader.hasTemporalData(),
		true
	);
}
