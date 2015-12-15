#include <gtest/gtest.h>

#include <string>

#include <Reader/DataReader/JsonReaderFactory.hpp>
#include <Reader/DataReader/AbstractJsonReader.hpp>
#include <Reader/DataReader/DataType.hpp>

TEST(TestJsonReaderFactory, ReadValidFiles) {
	AbstractJsonReader cityReader = JsonReaderFactory::createReader("res/test-data/cities.json");

	EXPECT_EQ(
		cityReader.getDataType(),
		DataType::CITIES
	);
	
	EXPECT_EQ(
		cityReader.hasTemporalData(),
		false
	);
}
