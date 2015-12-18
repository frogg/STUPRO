#include <gtest/gtest.h>

#include <string>

#include <Reader/DataReader/JsonReaderFactory.hpp>
#include <Reader/DataReader/JsonReader.hpp>
#include <Reader/DataReader/DataType.hpp>
#include <Reader/DataReader/DataPoints/DataPoint.hpp>
#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/CityDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/TweetDataPoint.hpp>

#include <vtkSmartPointer.h>
#include <iostream>

TEST(TestJsonReader, ReadCityData) {
	JsonReader cityReader = JsonReaderFactory::createReader("res/test-data/cities.json");
	EXPECT_EQ(
		cityReader.pointDataSet.getDataPoints().size(),
		4
	);
	
	const CityDataPoint* firstDataPoint = dynamic_cast<const CityDataPoint*>(
		cityReader.pointDataSet.getDataPoints().at(1)
	);
	
	EXPECT_EQ(
		firstDataPoint->getName().toStdString(),
		"Los Angeles"
	);
	
	EXPECT_EQ(
		firstDataPoint->getCoordinate().lat(),
		34.052223
	);
	
	EXPECT_EQ(
		firstDataPoint->getCoordinate().lon(),
		-118.242775
	);
}

TEST(TestJsonReader, ReadTwitterData) {
	JsonReader twitterReader = JsonReaderFactory::createReader("res/test-data/tweets.json");
	EXPECT_EQ(
		twitterReader.pointDataSet.getDataPoints().size(),
		3
	);
	
	const TweetDataPoint* testDataPoint = dynamic_cast<const TweetDataPoint*>(
		twitterReader.pointDataSet.getDataPoints().at(1)
	);
	
	EXPECT_EQ(
		testDataPoint->getAuthor().toStdString(),
		"elonmusk"
	);
	
	EXPECT_EQ(
		testDataPoint->getContent().toStdString(),
		"Is this working?"
	);
	
	EXPECT_EQ(
		testDataPoint->getTimestamp(),
		1439280065
	);
	
	EXPECT_EQ(
		testDataPoint->getCoordinate().lat(),
		34.052223
	);
	
	EXPECT_EQ(
		testDataPoint->getCoordinate().lon(),
		-118.242775
	);
}

/*TEST(TestJsonReader, TestShitty) {
    JsonReader jsonReader = JsonReaderFactory::createReader("res/test-data/cities.json");
    vtkSmartPointer<vtkPolyData> polyData = jsonReader.getVtkDataSet(9);
    EXPECT_EQ(false, true);
}*/