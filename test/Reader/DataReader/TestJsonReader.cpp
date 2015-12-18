#include <gtest/gtest.h>

#include <string>

#include <Reader/DataReader/JsonReaderFactory.hpp>
#include <Reader/DataReader/JsonReader.hpp>
#include <Reader/DataReader/DataType.hpp>

#include <vtkSmartPointer.h>

TEST(TestJsonReader, ReadCityData) {
	JsonReader cityReader = JsonReaderFactory::createReader("res/test-data/cities.json");
}

TEST(TestJsonReader, TestShitty) {
    JsonReader jsonReader = JsonReaderFactory::createReader("res/test-data/cities.json");
    vtkSmartPointer<vtkPolyData> polyData = jsonReader.getVtkDataSet(9);
    EXPECT_EQ(false, true);
}