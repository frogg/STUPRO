#include <gtest/gtest.h>

#include <string>

#include <Reader/DataReader/JsonReaderFactory.hpp>
#include <Reader/DataReader/JsonReader.hpp>
#include <Reader/DataReader/DataType.hpp>
#include <Reader/DataReader/DataPoints/DataPoint.hpp>
#include <Reader/DataReader/DataPoints/NonTemporalDataPoints/CityDataPoint.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/TweetDataPoint.hpp>

#include <Utils/Config/Configuration.hpp>

#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkStringArray.h>
#include <vtkAbstractArray.h>
#include <vtkDataArray.h>
#include <vtkTypeInt32Array.h>
#include <vtkDoubleArray.h>

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

TEST(TestJsonReader, TestPointCoordinatesInVtkPolyData) {
	JsonReader jsonReader = JsonReaderFactory::createReader("res/test-data/cities.json");
	
    vtkSmartPointer<vtkPolyData> polyData = jsonReader.getVtkDataSet(9);
	vtkSmartPointer<vtkPolyData> lessPolyData = jsonReader.getVtkDataSet(0);
    
	// Test the actual points along with their coordinates
	
	EXPECT_EQ(
		4,
		polyData->GetNumberOfPoints()
	);
	
	EXPECT_EQ(
		2,
		lessPolyData->GetNumberOfPoints()
	);
	
	double testPointCoordinates[3];
	polyData->GetPoint(2, testPointCoordinates);
	
	EXPECT_FLOAT_EQ(
		-122.418335,
		testPointCoordinates[0]
	);
	
	EXPECT_FLOAT_EQ(
		37.775,
		testPointCoordinates[1]
	);
	
	EXPECT_FLOAT_EQ(
		0,
		testPointCoordinates[2]
	);
}

TEST(TestJsonReader, WriteCitiesToVtkPolyData) {
    JsonReader jsonReader = JsonReaderFactory::createReader("res/test-data/cities.json");	
    vtkSmartPointer<vtkPolyData> polyData = jsonReader.getVtkDataSet(
		Configuration::getInstance().getInteger("dataReader.maximumPriority")
	);

	// Test the associated array of city names
	vtkSmartPointer<vtkAbstractArray> abstractCityNameArray = polyData->GetPointData()
		->GetAbstractArray("names");
	ASSERT_TRUE(abstractCityNameArray);
	vtkSmartPointer<vtkStringArray> cityNameArray = vtkStringArray::SafeDownCast(
		abstractCityNameArray
	);
	ASSERT_TRUE(cityNameArray);
	
	EXPECT_EQ(
		4,
		cityNameArray->GetNumberOfValues()
	);
	
	EXPECT_EQ(
		"San Francisco",
		cityNameArray->GetValue(2)
	);
	
	// Test the associated array of data point priorities
	vtkSmartPointer<vtkDataArray> abstractPriorityArray = polyData->GetPointData()
		->GetArray("priorities");
	ASSERT_TRUE(abstractPriorityArray);
	vtkSmartPointer<vtkTypeInt32Array> priorityArray = vtkTypeInt32Array::SafeDownCast(
		abstractPriorityArray
	);
	ASSERT_TRUE(priorityArray);
	
	EXPECT_EQ(
		4,
		priorityArray->GetNumberOfComponents()
	);
	
	EXPECT_EQ(
		Configuration::getInstance().getInteger("dataReader.maximumPriority") - 1,
		priorityArray->GetValue(2)
	);
}

TEST(TestJsonReader, WriteFlightsToVtkPolyData) {
    JsonReader jsonReader = JsonReaderFactory::createReader("res/test-data/flights.json");
    vtkSmartPointer<vtkPolyData> polyData = jsonReader.getVtkDataSet(
		Configuration::getInstance().getInteger("dataReader.maximumPriority")
	);

	// Test the associated array of destination coordinates
	vtkSmartPointer<vtkDataArray> abstractDestinationArray = polyData->GetPointData()
		->GetArray("destinations");
	ASSERT_TRUE(abstractDestinationArray);
	vtkSmartPointer<vtkDoubleArray> destinationArray = vtkDoubleArray::SafeDownCast(
		abstractDestinationArray
	);
	ASSERT_TRUE(destinationArray);
	
	EXPECT_EQ(
		2,
		destinationArray->GetNumberOfComponents()
	);
	
	EXPECT_FLOAT_EQ(
		34.052223,
		destinationArray->GetTuple2(0)[0]
	);
	
	EXPECT_FLOAT_EQ(
		-118.242775,
		destinationArray->GetTuple2(0)[1]
	);
	
	// Test the associated array of data point priorities
	vtkSmartPointer<vtkDataArray> abstractPriorityArray = polyData->GetPointData()
		->GetArray("priorities");
	ASSERT_TRUE(abstractPriorityArray);
	vtkSmartPointer<vtkTypeInt32Array> priorityArray = vtkTypeInt32Array::SafeDownCast(
		abstractPriorityArray
	);
	ASSERT_TRUE(priorityArray);
	
	EXPECT_EQ(
		1,
		priorityArray->GetNumberOfComponents()
	);
	
	EXPECT_EQ(
		Configuration::getInstance().getInteger("dataReader.maximumPriority"),
		priorityArray->GetValue(0)
	);
}