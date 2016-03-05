#include <gtest/gtest.h>

#include <Filter/TemporalAggregationFilter.h>
#include <Filter/TemporalInterpolationFilter.h>
#include <Filter/WindVelocityVectorCalculationFilter.h>
#include <Reader/vtkKronosReader.h>
#include <Reader/DataReader/JsonReader.hpp>
#include <Reader/DataReader/JsonReaderFactory.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

#include <vtkFloatArray.h>
#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>

TEST(TestWindVelocityVectorCalculationFilter, TestRawWindData) {
	// Read some test data from the test resources
	std::unique_ptr<JsonReader> jsonReader =
	    JsonReaderFactory::createReader("res/test-data/wind.json");
	vtkSmartPointer<vtkPolyData> inputDataSet = jsonReader->getVtkDataSet(0);

	ASSERT_EQ(1, inputDataSet->GetNumberOfPoints());

	// Set up the filter and its input
	vtkSmartPointer<WindVelocityVectorCalculationFilter> filter = WindVelocityVectorCalculationFilter::New();
	filter->SetInputData(0, inputDataSet);
	filter->GetInputInformation()->Set(Data::VTK_DATA_TYPE(), Data::WIND);
	filter->GetInputInformation()->Set(Data::VTK_DATA_STATE(), Data::RAW);
	filter->Update();

	// Test the filter's output which should contain a computed velocity vector
	vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
	outputDataSet->ShallowCopy(vtkPolyData::SafeDownCast(filter->GetOutputDataObject(0)));
	
	// Extract the filter's output array
	vtkSmartPointer<vtkFloatArray> windVelocityArray = vtkFloatArray::SafeDownCast(
				outputDataSet->GetPointData()->GetArray("velocity"));
	ASSERT_TRUE(windVelocityArray);
	
	// Finally, test the values against some pre-computed ones
	EXPECT_NEAR(0.346, windVelocityArray->GetTuple3(0)[0], 0.001);
	EXPECT_NEAR(-0.200, windVelocityArray->GetTuple3(0)[1], 0.001);
	EXPECT_FLOAT_EQ(0, windVelocityArray->GetTuple3(0)[2]);
}

TEST(TestWindVelocityVectorCalculationFilter, TestInterpolatedWindData) {
	// Read some test data
	vtkSmartPointer<vtkKronosReader> kronosReader = vtkSmartPointer<vtkKronosReader>::New();
	kronosReader->SetFileName("res/test-data/temporal-interpolation-test/wind-test-data.kJson", false);

	// Set up the filter and its input
	vtkSmartPointer<TemporalInterpolationFilter> interpolationFilter = TemporalInterpolationFilter::New();
	interpolationFilter->SetInputConnection(kronosReader->GetOutputPort());
	interpolationFilter->GetInputInformation()->Set(Data::VTK_DATA_TYPE(), Data::WIND);
	interpolationFilter->GetInputInformation()->Set(Data::VTK_TIME_RESOLUTION(), 1);
	
	// Send the interpolation filter's output through the wind velocity vector calculation filter
	vtkSmartPointer<WindVelocityVectorCalculationFilter> filter = WindVelocityVectorCalculationFilter::New();
	filter->SetInputConnection(interpolationFilter->GetOutputPort());

	// Test integral values
	QList<Vector3<float>> windVelocitiesOfFirstPoint = QList<Vector3<float>>() << Vector3<float>
	                   (0.0052335961, 0.099862956, 0) << Vector3<float>(1.3167129, -0.85508329,
	                           0) << Vector3<float>(-0.027239431, -0.053460389, 0) << Vector3<float>(-1.5553, 0.3306, 0);
	QList<Vector3<float>> windVelocitiesOfSecondPoint = QList<Vector3<float>>() << Vector3<float>
	                   (-0.49950644, 0.66286749, 0) << Vector3<float>(-0.49950644, 0.66286749,
	                           0) << Vector3<float>(-0.49950644, 0.66286749, 0) << Vector3<float>(-0.49950644, 0.66286749, 0);

	for (int t = 0; t < windVelocitiesOfFirstPoint.size() - 1; t++) {
		filter->GetOutputInformation(0)->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), t);
		filter->Update();

		// Run the filter on the input data
		vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
		outputDataSet->ShallowCopy(vtkPolyData::SafeDownCast(filter->GetOutputDataObject(0)));

		ASSERT_TRUE(outputDataSet);

		// Extract the filter's output
		vtkSmartPointer<vtkFloatArray> windVelocityArray = vtkFloatArray::SafeDownCast(
		            outputDataSet->GetPointData()->GetArray("velocity"));
		ASSERT_TRUE(windVelocityArray);

		EXPECT_FLOAT_EQ(windVelocitiesOfFirstPoint[t].x, windVelocityArray->GetTuple3(0)[0]);
		EXPECT_FLOAT_EQ(windVelocitiesOfFirstPoint[t].y, windVelocityArray->GetTuple3(0)[1]);
		EXPECT_FLOAT_EQ(windVelocitiesOfFirstPoint[t].z, windVelocityArray->GetTuple3(0)[2]);

		EXPECT_FLOAT_EQ(windVelocitiesOfSecondPoint[t].x, windVelocityArray->GetTuple3(1)[0]);
		EXPECT_FLOAT_EQ(windVelocitiesOfSecondPoint[t].y, windVelocityArray->GetTuple3(1)[1]);
		EXPECT_FLOAT_EQ(windVelocitiesOfSecondPoint[t].z, windVelocityArray->GetTuple3(1)[2]);
	}

	// Test an intermediate value
	filter->GetOutputInformation(0)->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), 0.5);
	filter->Update();

	// Run the filter on the input data
	vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
	outputDataSet->ShallowCopy(vtkPolyData::SafeDownCast(filter->GetOutputDataObject(0)));

	ASSERT_TRUE(outputDataSet);

	// Extract the filter's output
	vtkSmartPointer<vtkFloatArray> windVelocityArray = vtkFloatArray::SafeDownCast(
	            outputDataSet->GetPointData()->GetArray("velocity"));
	ASSERT_TRUE(windVelocityArray);

	EXPECT_FLOAT_EQ(-0.49950641, windVelocityArray->GetTuple3(1)[0]);
	EXPECT_FLOAT_EQ(0.66286743, windVelocityArray->GetTuple3(1)[1]);
	EXPECT_FLOAT_EQ(0, windVelocityArray->GetTuple3(1)[2]);

	EXPECT_FLOAT_EQ(0.74399048, windVelocityArray->GetTuple3(0)[0]);
	EXPECT_FLOAT_EQ(0.37908211, windVelocityArray->GetTuple3(0)[1]);
	EXPECT_FLOAT_EQ(0, windVelocityArray->GetTuple3(0)[2]);
}

TEST(TestWindVelocityVectorCalculationFilter, TestAggregatedWindData) {
	// Read some test data
	std::unique_ptr<JsonReader> jsonReader =
		JsonReaderFactory::createReader("res/test-data/temporal-aggregation-test/wind-test-data.kJson");
	vtkSmartPointer<vtkPolyData> inputDataSet = jsonReader->getVtkDataSet(0);

	// Set up the filter and its input
	vtkSmartPointer<TemporalAggregationFilter> aggregationFilter = TemporalAggregationFilter::New();
	aggregationFilter->SetInputData(0, inputDataSet);
	aggregationFilter->GetInputInformation()->Set(Data::VTK_DATA_TYPE(), Data::WIND);
	aggregationFilter->GetInputInformation()->Set(Data::VTK_DATA_STATE(), Data::RAW);
	aggregationFilter->GetInputInformation()->Set(Data::VTK_TIME_RESOLUTION(), 1);
	aggregationFilter->Update();
	
	// Send the aggregation filter's output through the wind velocity vector calculation filter
	vtkSmartPointer<WindVelocityVectorCalculationFilter> filter = WindVelocityVectorCalculationFilter::New();
	filter->SetInputConnection(aggregationFilter->GetOutputPort());
	filter->Update();

	// Run the filter on the input data
	vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
	outputDataSet->ShallowCopy(vtkPolyData::SafeDownCast(filter->GetOutputDataObject(0)));

	ASSERT_TRUE(outputDataSet);

	// Extract the filter's outputs
	vtkSmartPointer<vtkDataArray> abstractWindVelocitiesArray = outputDataSet->GetPointData()
			->GetArray("velocity");
	vtkSmartPointer<vtkFloatArray> windVelocitiesArray = vtkFloatArray::SafeDownCast(
				abstractWindVelocitiesArray);
	ASSERT_TRUE(windVelocitiesArray);

	// Test the wind velocities that the filters have produced in cooperation
	EXPECT_EQ(2, windVelocitiesArray->GetNumberOfTuples());
	EXPECT_EQ(3, windVelocitiesArray->GetNumberOfComponents());

	EXPECT_FLOAT_EQ(-0.70285702, windVelocitiesArray->GetTuple3(0)[0]);
	EXPECT_FLOAT_EQ(-0.3428061, windVelocitiesArray->GetTuple3(0)[1]);
	EXPECT_FLOAT_EQ(0, windVelocitiesArray->GetTuple3(0)[2]);

	EXPECT_FLOAT_EQ(0.36462292, windVelocitiesArray->GetTuple3(1)[0]);
	EXPECT_FLOAT_EQ(0.68575597, windVelocitiesArray->GetTuple3(1)[1]);
	EXPECT_FLOAT_EQ(0, windVelocitiesArray->GetTuple3(1)[2]);
}