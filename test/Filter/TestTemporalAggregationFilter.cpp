#include <gtest/gtest.h>

#include <Filter/TemporalAggregationFilter.h>
#include <Reader/DataReader/JsonReader.hpp>
#include <Reader/DataReader/JsonReaderFactory.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

#include <vtkTypeFloat32Array.h>

TEST(TestTemporalAggregationFilter, TestTemperatureData) {
    // Read some test data
	std::unique_ptr<JsonReader> jsonReader = JsonReaderFactory::createReader("res/test-data/temporal-aggregation-test/temperature-test-data.kJson");
	vtkSmartPointer<vtkPolyData> inputDataSet = jsonReader->getVtkDataSet(0);
    
    // Set up the filter and its input
    vtkSmartPointer<TemporalAggregationFilter> filter = TemporalAggregationFilter::New();
    filter->SetInputData(0, inputDataSet);
    filter->GetInputInformation()->Set(Data::VTK_DATA_TYPE(), Data::TEMPERATURE);
    filter->GetInputInformation()->Set(Data::VTK_TIME_RESOLUTION(), 1);
    filter->Update();

    // Run the filter on the input data
    vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
    outputDataSet->ShallowCopy(filter->GetPolyDataOutput());

    ASSERT_TRUE(outputDataSet);
    
    vtkSmartPointer<vtkDataArray> abstractTemperatureArray = outputDataSet->GetPointData()
            ->GetArray("Average Temperatures");
    vtkSmartPointer<vtkTypeFloat32Array> temperatureArray = vtkTypeFloat32Array::SafeDownCast(
                abstractTemperatureArray);
    ASSERT_TRUE(temperatureArray);
    
	// Test the data that the filter has produced
	EXPECT_EQ(2, temperatureArray->GetNumberOfTuples());
	EXPECT_FLOAT_EQ(8.874, *temperatureArray->GetTuple(0));
	EXPECT_FLOAT_EQ(16.42, *temperatureArray->GetTuple(1));
}