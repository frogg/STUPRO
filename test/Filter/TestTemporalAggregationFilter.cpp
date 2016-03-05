#include <gtest/gtest.h>

#include <Filter/TemporalAggregationFilter.h>
#include <Reader/DataReader/JsonReader.hpp>
#include <Reader/DataReader/JsonReaderFactory.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

#include <vtkFloatArray.h>
#include <vtkInformation.h>

TEST(TestTemporalAggregationFilter, TestTemperatureData) {
	// Read some test data
	std::unique_ptr<JsonReader> jsonReader =
	    JsonReaderFactory::createReader("res/test-data/temporal-aggregation-test/temperature-test-data.kJson");
	vtkSmartPointer<vtkPolyData> inputDataSet = jsonReader->getVtkDataSet(0);

	// Set up the filter and its input
	vtkSmartPointer<TemporalAggregationFilter> filter = TemporalAggregationFilter::New();
	filter->SetInputData(0, inputDataSet);
	filter->GetInputInformation()->Set(Data::VTK_DATA_TYPE(), Data::TEMPERATURE);
	filter->GetInputInformation()->Set(Data::VTK_DATA_STATE(), Data::RAW);
	filter->GetInputInformation()->Set(Data::VTK_TIME_RESOLUTION(), 1);
	filter->Update();

	// Run the filter on the input data
	vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
	outputDataSet->ShallowCopy(filter->GetPolyDataOutput());

	ASSERT_TRUE(outputDataSet);

	// Extract the filter's output
	vtkSmartPointer<vtkDataArray> abstractTemperatureArray = outputDataSet->GetPointData()
	        ->GetArray("Average Temperatures");
	vtkSmartPointer<vtkFloatArray> temperatureArray = vtkFloatArray::SafeDownCast(
	            abstractTemperatureArray);
	ASSERT_TRUE(temperatureArray);

	// Test the data that the filter has produced
	EXPECT_EQ(2, temperatureArray->GetNumberOfTuples());
	EXPECT_FLOAT_EQ(8.874, *temperatureArray->GetTuple(0));
	EXPECT_FLOAT_EQ(16.42, *temperatureArray->GetTuple(1));
}

TEST(TestTemporalAggregationFilter, TestCloudCoverageData) {
	// Read some test data
	std::unique_ptr<JsonReader> jsonReader =
	    JsonReaderFactory::createReader("res/test-data/temporal-aggregation-test/cloud-coverage-test-data.kJson");
	vtkSmartPointer<vtkPolyData> inputDataSet = jsonReader->getVtkDataSet(0);

	// Set up the filter and its input
	vtkSmartPointer<TemporalAggregationFilter> filter = TemporalAggregationFilter::New();
	filter->SetInputData(0, inputDataSet);
	filter->GetInputInformation()->Set(Data::VTK_DATA_TYPE(), Data::CLOUD_COVERAGE);
	filter->GetInputInformation()->Set(Data::VTK_DATA_STATE(), Data::RAW);
	filter->GetInputInformation()->Set(Data::VTK_TIME_RESOLUTION(), 1);
	filter->Update();

	// Run the filter on the input data
	vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
	outputDataSet->ShallowCopy(filter->GetPolyDataOutput());

	ASSERT_TRUE(outputDataSet);

	// Extract the filter's output
	vtkSmartPointer<vtkDataArray> abstractCloudCoverageArray = outputDataSet->GetPointData()
	        ->GetArray("Average Cloud Coverage Values");
	vtkSmartPointer<vtkFloatArray> cloudCoverageArray = vtkFloatArray::SafeDownCast(
	            abstractCloudCoverageArray);
	ASSERT_TRUE(cloudCoverageArray);

	// Test the data that the filter has produced
	EXPECT_EQ(2, cloudCoverageArray->GetNumberOfTuples());
	EXPECT_FLOAT_EQ(0.626, *cloudCoverageArray->GetTuple(0));
	EXPECT_FLOAT_EQ(0.17, *cloudCoverageArray->GetTuple(1));
}

TEST(TestTemporalAggregationFilter, TestWindData) {
	// Read some test data
	std::unique_ptr<JsonReader> jsonReader =
	    JsonReaderFactory::createReader("res/test-data/temporal-aggregation-test/wind-test-data.kJson");
	vtkSmartPointer<vtkPolyData> inputDataSet = jsonReader->getVtkDataSet(0);

	// Set up the filter and its input
	vtkSmartPointer<TemporalAggregationFilter> filter = TemporalAggregationFilter::New();
	filter->SetInputData(0, inputDataSet);
	filter->GetInputInformation()->Set(Data::VTK_DATA_TYPE(), Data::WIND);
	filter->GetInputInformation()->Set(Data::VTK_DATA_STATE(), Data::RAW);
	filter->GetInputInformation()->Set(Data::VTK_TIME_RESOLUTION(), 1);
	filter->Update();

	// Run the filter on the input data
	vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
	outputDataSet->ShallowCopy(filter->GetPolyDataOutput());

	ASSERT_TRUE(outputDataSet);

	// Extract the filter's outputs
	vtkSmartPointer<vtkDataArray> abstractWindSpeedsArray = outputDataSet->GetPointData()
	        ->GetArray("Average Wind Speeds");
	vtkSmartPointer<vtkFloatArray> windSpeedsArray = vtkFloatArray::SafeDownCast(
	            abstractWindSpeedsArray);
	ASSERT_TRUE(windSpeedsArray);

	vtkSmartPointer<vtkDataArray> abstractWindDirectionsArray = outputDataSet->GetPointData()
	        ->GetArray("Average Wind Directions");
	vtkSmartPointer<vtkFloatArray> windDirectionsArray = vtkFloatArray::SafeDownCast(
	            abstractWindDirectionsArray);
	ASSERT_TRUE(windDirectionsArray);

	// Test the data that the filter has produced

	// First, test the wind speeds
	EXPECT_EQ(2, windSpeedsArray->GetNumberOfTuples());
	EXPECT_FLOAT_EQ(0.782, *windSpeedsArray->GetTuple(0));
	EXPECT_FLOAT_EQ(0.7766667, *windSpeedsArray->GetTuple(1));

	// Secondly, test the wind directions
	EXPECT_EQ(2, windDirectionsArray->GetNumberOfTuples());
	EXPECT_FLOAT_EQ(244, *windDirectionsArray->GetTuple(0));
	EXPECT_FLOAT_EQ(28, *windDirectionsArray->GetTuple(1));
}