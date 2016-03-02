#include <gtest/gtest.h>

#include <Filter/TemporalInterpolationFilter.h>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>
#include <Reader/vtkKronosReader.h>
#include <Utils/Math/Vector3.hpp>

#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkPointData.h>

#include <qlist.h>

TEST(TestTemporalInterpolationFilter, TestPrecipitationData) {
	// Read some test data
	vtkSmartPointer<vtkKronosReader> kronosReader = vtkSmartPointer<vtkKronosReader>::New();
	kronosReader->SetFileName("res/test-data/temporal-interpolation-test/precipitation-test-data.kJson");

	// Set up the filter and its input
	vtkSmartPointer<TemporalInterpolationFilter> filter = TemporalInterpolationFilter::New();
	filter->SetInputConnection(kronosReader->GetOutputPort());
	filter->GetInputInformation()->Set(Data::VTK_DATA_TYPE(), Data::TEMPERATURE);
	filter->GetInputInformation()->Set(Data::VTK_TIME_RESOLUTION(), 1);

	// Test integral values
	QList<float> precipitationRatesOfFirstPoint = QList<float>() << 4.45 << 7.86 << 3.72 << 4.76;
	QList<int> precipitationTypesOfFirstPoint = QList<int>() << PrecipitationDataPoint::RAIN <<
	        PrecipitationDataPoint::SNOW << PrecipitationDataPoint::SLEET << PrecipitationDataPoint::SNOW;

	QList<float> precipitationRatesOfSecondPoint = QList<float>() << 6.49 << 6.1133332 << 5.7366667 <<
	        5.36;
	QList<int> precipitationTypesOfSecondPoint = QList<int>() << PrecipitationDataPoint::NONE <<
	        PrecipitationDataPoint::NONE << PrecipitationDataPoint::HAIL << PrecipitationDataPoint::HAIL;

	for (int t = 0; t < precipitationRatesOfFirstPoint.size() - 1; t++) {
		filter->GetOutputInformation(0)->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), t);
		filter->Update();

		// Run the filter on the input data
		vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
		outputDataSet->ShallowCopy(filter->GetPolyDataOutput());

		ASSERT_TRUE(outputDataSet);

		// Extract the filter's output
		vtkSmartPointer<vtkFloatArray> precipitationRateArray = vtkFloatArray::SafeDownCast(
		            outputDataSet->GetPointData()->GetArray("precipitationRates"));
		ASSERT_TRUE(precipitationRateArray);

		vtkSmartPointer<vtkIntArray> precipitationTypeArray = vtkIntArray::SafeDownCast(
		            outputDataSet->GetPointData()->GetArray("precipitationTypes"));
		ASSERT_TRUE(precipitationTypeArray);

		EXPECT_FLOAT_EQ(precipitationRatesOfFirstPoint[t], precipitationRateArray->GetTuple1(0));
		EXPECT_FLOAT_EQ(precipitationTypesOfFirstPoint[t], precipitationTypeArray->GetTuple1(0));

		EXPECT_FLOAT_EQ(precipitationRatesOfSecondPoint[t], precipitationRateArray->GetTuple1(1));
		EXPECT_FLOAT_EQ(precipitationTypesOfSecondPoint[t], precipitationTypeArray->GetTuple1(1));
	}

	// Test an intermediate value
	filter->GetOutputInformation(0)->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), 0.5);
	filter->Update();

	// Run the filter on the input data
	vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
	outputDataSet->ShallowCopy(filter->GetPolyDataOutput());

	ASSERT_TRUE(outputDataSet);

	// Extract the filter's output
	vtkSmartPointer<vtkFloatArray> precipitationRateArray = vtkFloatArray::SafeDownCast(
	            outputDataSet->GetPointData()->GetArray("precipitationRates"));
	ASSERT_TRUE(precipitationRateArray);

	vtkSmartPointer<vtkIntArray> precipitationTypeArray = vtkIntArray::SafeDownCast(
	            outputDataSet->GetPointData()->GetArray("precipitationTypes"));
	ASSERT_TRUE(precipitationTypeArray);

	EXPECT_FLOAT_EQ(6.1549997, precipitationRateArray->GetTuple1(0));
	EXPECT_FLOAT_EQ(PrecipitationDataPoint::SNOW, precipitationTypeArray->GetTuple1(0));

	EXPECT_FLOAT_EQ(6.3016663, precipitationRateArray->GetTuple1(1));
	EXPECT_FLOAT_EQ(PrecipitationDataPoint::NONE, precipitationTypeArray->GetTuple1(1));

	kronosReader->abortCaching();
}

TEST(TestTemporalInterpolationFilter, TestTemperatureData) {
	// Read some test data
	vtkSmartPointer<vtkKronosReader> kronosReader = vtkSmartPointer<vtkKronosReader>::New();
	kronosReader->SetFileName("res/test-data/temporal-interpolation-test/temperature-test-data.kJson");

	// Set up the filter and its input
	vtkSmartPointer<TemporalInterpolationFilter> filter = TemporalInterpolationFilter::New();
	filter->SetInputConnection(kronosReader->GetOutputPort());
	filter->GetInputInformation()->Set(Data::VTK_DATA_TYPE(), Data::TEMPERATURE);
	filter->GetInputInformation()->Set(Data::VTK_TIME_RESOLUTION(), 1);

	// Test integral values
	QList<float> temperaturesOfFirstPoint = QList<float>() << 20.9 << 23.38 << 4.17 << 9.12;
	QList<float> temperaturesOfSecondPoint = QList<float>() << 18.8 << 18.8 << 18.8 << 18.8;

	for (int t = 0; t < temperaturesOfFirstPoint.size() - 1; t++) {
		filter->GetOutputInformation(0)->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), t);
		filter->Update();

		// Run the filter on the input data
		vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
		outputDataSet->ShallowCopy(filter->GetPolyDataOutput());

		ASSERT_TRUE(outputDataSet);

		// Extract the filter's output
		vtkSmartPointer<vtkFloatArray> temperatureArray = vtkFloatArray::SafeDownCast(
		            outputDataSet->GetPointData()->GetArray("temperatures"));
		ASSERT_TRUE(temperatureArray);

		EXPECT_FLOAT_EQ(temperaturesOfFirstPoint[t], temperatureArray->GetTuple1(0));
		EXPECT_FLOAT_EQ(temperaturesOfSecondPoint[t], temperatureArray->GetTuple1(1));
	}

	// Test an intermediate value
	filter->GetOutputInformation(0)->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), 0.5);
	filter->Update();

	// Run the filter on the input data
	vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
	outputDataSet->ShallowCopy(filter->GetPolyDataOutput());

	ASSERT_TRUE(outputDataSet);

	// Extract the filter's output
	vtkSmartPointer<vtkFloatArray> temperatureArray = vtkFloatArray::SafeDownCast(
	            outputDataSet->GetPointData()->GetArray("temperatures"));
	ASSERT_TRUE(temperatureArray);

	EXPECT_FLOAT_EQ(22.14, temperatureArray->GetTuple1(0));
	EXPECT_FLOAT_EQ(18.8, temperatureArray->GetTuple1(1));

	kronosReader->abortCaching();
}

TEST(TestTemporalInterpolationFilter, TestWindData) {
	// Read some test data
	vtkSmartPointer<vtkKronosReader> kronosReader = vtkSmartPointer<vtkKronosReader>::New();
	kronosReader->SetFileName("res/test-data/temporal-interpolation-test/wind-test-data.kJson");

	// Set up the filter and its input
	vtkSmartPointer<TemporalInterpolationFilter> filter = TemporalInterpolationFilter::New();
	filter->SetInputConnection(kronosReader->GetOutputPort());
	filter->GetInputInformation()->Set(Data::VTK_DATA_TYPE(), Data::TEMPERATURE);
	filter->GetInputInformation()->Set(Data::VTK_TIME_RESOLUTION(), 1);

	// Test integral values
	QList<float> windSpeedsOfFirstPoint = QList<float>() << 0.1 << 1.57 << 0.06 << 1.59;
	QList<int> windBearingsOfFirstPoint = QList<int>() << 3 << 123 << 207 << 282;
	QList<Vector3<float>> windVelocitiesOfFirstPoint = QList<Vector3<float>>() << Vector3<float>
	                   (0.0052335961, 0.099862956, 0) << Vector3<float>(1.3167129, -0.85508329,
	                           0) << Vector3<float>(-0.027239431, -0.053460389, 0) << Vector3<float>(-1.5553, 0.3306, 0);

	QList<float> windSpeedsOfSecondPoint = QList<float>() << 0.83 << 0.83 << 0.83 << 0.83;
	QList<int> windBearingsOfSecondPoint = QList<int>() << 323 << 323 << 323 << 323;
	QList<Vector3<float>> windVelocitiesOfSecondPoint = QList<Vector3<float>>() << Vector3<float>
	                   (-0.49950644, 0.66286749, 0) << Vector3<float>(-0.49950644, 0.66286749,
	                           0) << Vector3<float>(-0.49950644, 0.66286749, 0) << Vector3<float>(-0.49950644, 0.66286749, 0);

	for (int t = 0; t < windSpeedsOfFirstPoint.size() - 1; t++) {
		filter->GetOutputInformation(0)->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), t);
		filter->Update();

		// Run the filter on the input data
		vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
		outputDataSet->ShallowCopy(filter->GetPolyDataOutput());

		ASSERT_TRUE(outputDataSet);

		// Extract the filter's output
		vtkSmartPointer<vtkFloatArray> windSpeedArray = vtkFloatArray::SafeDownCast(
		            outputDataSet->GetPointData()->GetArray("speeds"));
		ASSERT_TRUE(windSpeedArray);

		vtkSmartPointer<vtkFloatArray> windBearingArray = vtkFloatArray::SafeDownCast(
		            outputDataSet->GetPointData()->GetArray("directions"));
		ASSERT_TRUE(windBearingArray);

		vtkSmartPointer<vtkFloatArray> windVelocityArray = vtkFloatArray::SafeDownCast(
		            outputDataSet->GetPointData()->GetArray("velocity"));
		ASSERT_TRUE(windVelocityArray);

		EXPECT_FLOAT_EQ(windSpeedsOfFirstPoint[t], windSpeedArray->GetTuple1(0));
		EXPECT_FLOAT_EQ(windBearingsOfFirstPoint[t], windBearingArray->GetTuple1(0));
		EXPECT_FLOAT_EQ(windVelocitiesOfFirstPoint[t].x, windVelocityArray->GetTuple3(0)[0]);
		EXPECT_FLOAT_EQ(windVelocitiesOfFirstPoint[t].y, windVelocityArray->GetTuple3(0)[1]);
		EXPECT_FLOAT_EQ(windVelocitiesOfFirstPoint[t].z, windVelocityArray->GetTuple3(0)[2]);

		EXPECT_FLOAT_EQ(windSpeedsOfSecondPoint[t], windSpeedArray->GetTuple1(1));
		EXPECT_FLOAT_EQ(windBearingsOfSecondPoint[t], windBearingArray->GetTuple1(1));
		EXPECT_FLOAT_EQ(windVelocitiesOfSecondPoint[t].x, windVelocityArray->GetTuple3(1)[0]);
		EXPECT_FLOAT_EQ(windVelocitiesOfSecondPoint[t].y, windVelocityArray->GetTuple3(1)[1]);
		EXPECT_FLOAT_EQ(windVelocitiesOfSecondPoint[t].z, windVelocityArray->GetTuple3(1)[2]);
	}

	// Test an intermediate value
	filter->GetOutputInformation(0)->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), 0.5);
	filter->Update();

	// Run the filter on the input data
	vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
	outputDataSet->ShallowCopy(filter->GetPolyDataOutput());

	ASSERT_TRUE(outputDataSet);

	// Extract the filter's output
	vtkSmartPointer<vtkFloatArray> windSpeedArray = vtkFloatArray::SafeDownCast(
	            outputDataSet->GetPointData()->GetArray("speeds"));
	ASSERT_TRUE(windSpeedArray);

	vtkSmartPointer<vtkFloatArray> windBearingArray = vtkFloatArray::SafeDownCast(
	            outputDataSet->GetPointData()->GetArray("directions"));
	ASSERT_TRUE(windBearingArray);

	vtkSmartPointer<vtkFloatArray> windVelocityArray = vtkFloatArray::SafeDownCast(
	            outputDataSet->GetPointData()->GetArray("velocity"));
	ASSERT_TRUE(windVelocityArray);

	EXPECT_FLOAT_EQ(0.83, windSpeedArray->GetTuple1(1));
	EXPECT_FLOAT_EQ(323, windBearingArray->GetTuple1(1));
	EXPECT_FLOAT_EQ(-0.49950641, windVelocityArray->GetTuple3(1)[0]);
	EXPECT_FLOAT_EQ(0.66286743, windVelocityArray->GetTuple3(1)[1]);
	EXPECT_FLOAT_EQ(0, windVelocityArray->GetTuple3(1)[2]);

	EXPECT_FLOAT_EQ(0.835, windSpeedArray->GetTuple1(0));
	EXPECT_FLOAT_EQ(63, windBearingArray->GetTuple1(0));
	EXPECT_FLOAT_EQ(0.74399048, windVelocityArray->GetTuple3(0)[0]);
	EXPECT_FLOAT_EQ(0.37908211, windVelocityArray->GetTuple3(0)[1]);
	EXPECT_FLOAT_EQ(0, windVelocityArray->GetTuple3(0)[2]);

	kronosReader->abortCaching();
}

TEST(TestTemporalInterpolationFilter, TestCloudCoverageData) {
	// Read some test data
	vtkSmartPointer<vtkKronosReader> kronosReader = vtkSmartPointer<vtkKronosReader>::New();
	kronosReader->SetFileName("res/test-data/temporal-interpolation-test/cloud-coverage-test-data.kJson");

	// Set up the filter and its input
	vtkSmartPointer<TemporalInterpolationFilter> filter = TemporalInterpolationFilter::New();
	filter->SetInputConnection(kronosReader->GetOutputPort());
	filter->GetInputInformation()->Set(Data::VTK_DATA_TYPE(), Data::TEMPERATURE);
	filter->GetInputInformation()->Set(Data::VTK_TIME_RESOLUTION(), 1);

	// Test integral values
	QList<float> coverageValuesOfFirstPoint = QList<float>() << 0.43 << 0.86 << 0.89 << 0.35;
	QList<float> coverageValuesOfSecondPoint = QList<float>() << 0.63 << 0.63 << 0.63 << 0.63;

	for (int t = 0; t < coverageValuesOfFirstPoint.size() - 1; t++) {
		filter->GetOutputInformation(0)->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), t);
		filter->Update();

		// Run the filter on the input data
		vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
		outputDataSet->ShallowCopy(filter->GetPolyDataOutput());

		ASSERT_TRUE(outputDataSet);

		// Extract the filter's output
		vtkSmartPointer<vtkFloatArray> cloudCoverageArray = vtkFloatArray::SafeDownCast(
		            outputDataSet->GetPointData()->GetArray("cloudCovers"));
		ASSERT_TRUE(cloudCoverageArray);

		EXPECT_FLOAT_EQ(coverageValuesOfFirstPoint[t], cloudCoverageArray->GetTuple1(0));
		EXPECT_FLOAT_EQ(coverageValuesOfSecondPoint[t], cloudCoverageArray->GetTuple1(1));
	}

	// Test an intermediate value
	filter->GetOutputInformation(0)->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), 0.5);
	filter->Update();

	// Run the filter on the input data
	vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
	outputDataSet->ShallowCopy(filter->GetPolyDataOutput());

	ASSERT_TRUE(outputDataSet);

	// Extract the filter's output
	vtkSmartPointer<vtkFloatArray> cloudCoverageArray = vtkFloatArray::SafeDownCast(
	            outputDataSet->GetPointData()->GetArray("cloudCovers"));
	ASSERT_TRUE(cloudCoverageArray);

	EXPECT_FLOAT_EQ(0.645, cloudCoverageArray->GetTuple1(0));
	EXPECT_FLOAT_EQ(0.63, cloudCoverageArray->GetTuple1(1));

	kronosReader->abortCaching();
}
