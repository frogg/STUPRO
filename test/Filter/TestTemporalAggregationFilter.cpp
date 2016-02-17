#include <gtest/gtest.h>

#include <Filter/TemporalAggregationFilter.h>
#include <Reader/DataReader/JsonReader.hpp>
#include <Reader/DataReader/JsonReaderFactory.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

#include <vtkTypeFloat32Array.h>

#include <iostream>

TEST(TestTemporalAggregationFilter, TestPrecipitationData) {
    // Read some test data
	std::unique_ptr<JsonReader> jsonReader = JsonReaderFactory::createReader("res/test-data/temporal-aggregation-test/precipitation-test-data.kJson");
	vtkSmartPointer<vtkPolyData> inputDataSet = jsonReader->getVtkDataSet(0);
    
    // Set up the filter and its input
    vtkSmartPointer<TemporalAggregationFilter> filter = TemporalAggregationFilter::New();
    filter->SetInputData(0, inputDataSet);
    filter->GetInputInformation()->Set(Data::VTK_DATA_TYPE(), Data::PRECIPITATION);
    filter->GetInputInformation()->Set(Data::VTK_TIME_RESOLUTION(), 1);
    filter->Update();

    // Run the filter on the input data
    vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
    outputDataSet->ShallowCopy(filter->GetPolyDataOutput());

    ASSERT_TRUE(outputDataSet);
    
    vtkSmartPointer<vtkDataArray> abstractPrecipitationAmountArray = outputDataSet->GetPointData()
            ->GetArray("Accumulated Precipitation Amounts");
    vtkSmartPointer<vtkTypeFloat32Array> precipitationAmountArray = vtkTypeFloat32Array::SafeDownCast(
                abstractPrecipitationAmountArray);
    ASSERT_TRUE(precipitationAmountArray);
    
    std::cout << *precipitationAmountArray->GetTuple(0) << std::endl;
}