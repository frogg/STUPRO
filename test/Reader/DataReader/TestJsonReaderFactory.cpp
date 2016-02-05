#include <gtest/gtest.h>

#include <string>
#include <memory>

#include <Reader/DataReader/JsonReaderFactory.hpp>
#include <Reader/DataReader/JsonReader.hpp>
#include <Reader/DataReader/Data.hpp>

TEST(TestJsonReaderFactory, ReadValidFiles) {
	// Test reading some city data
	std::unique_ptr<JsonReader> cityReader =
	    JsonReaderFactory::createReader("res/test-data/cities.json");

	EXPECT_EQ(
	    cityReader->getDataType(),
<<<<<<< HEAD
	    DataType::CITIES
=======
	    Data::CITIES
>>>>>>> develop
	);

	EXPECT_EQ(
	    cityReader->hasTemporalData(),
	    false
	);

	// Test reading some tweets
	std::unique_ptr<JsonReader> tweetReader =
	    JsonReaderFactory::createReader("res/test-data/tweets.json");

	EXPECT_EQ(
	    tweetReader->getDataType(),
<<<<<<< HEAD
	    DataType::TWEETS
=======
	    Data::TWEETS
>>>>>>> develop
	);

	EXPECT_EQ(
	    tweetReader->hasTemporalData(),
	    true
	);
}

TEST(TestJsonReaderFactory, ReadInvalidFiles) {
	EXPECT_THROW(
	    JsonReaderFactory::createReader("res/test-data/non-existant.json"),
	    JsonReaderFileOpenException
	);

	EXPECT_THROW(
	    JsonReaderFactory::createReader("res/test-data/invalid.json"),
	    JsonReaderParseException
	);
}