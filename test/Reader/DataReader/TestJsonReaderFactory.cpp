#include <gtest/gtest.h>

#include <string>

#include <Reader/DataReader/JsonReaderFactory.hpp>
#include <Reader/DataReader/JsonReader.hpp>
#include <Reader/DataReader/DataType.hpp>

TEST(TestJsonReaderFactory, ReadValidFiles) {
	// Test reading some city data
	JsonReader cityReader = JsonReaderFactory::createReader("res/test-data/cities.json");

	EXPECT_EQ(
		cityReader.getDataType(),
		DataType::CITIES
	);
	
	EXPECT_EQ(
		cityReader.hasTemporalData(),
		false
	);
	
	// Test reading some tweets
	JsonReader tweetReader = JsonReaderFactory::createReader("res/test-data/tweets.json");

	EXPECT_EQ(
		tweetReader.getDataType(),
		DataType::TWEETS
	);
	
	EXPECT_EQ(
		tweetReader.hasTemporalData(),
		true
	);
}
