#include <gtest/gtest.h>

#include <string>

#include <Reader/DataReader/JsonReaderFactory.hpp>
#include <Reader/DataReader/JsonReader.hpp>
#include <Reader/DataReader/DataType.hpp>

TEST(TestJsonReader, ReadCityData) {
	JsonReader cityReader = JsonReaderFactory::createReader("res/test-data/cities.json");
}