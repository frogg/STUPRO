#include <gtest/gtest.h>

#include <Filter/TemporalInterpolationFilter.h>
#include <Reader/vtkKronosReader.h>

#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkPointData.h>

#include <qlist.h>

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
		vtkSmartPointer<vtkFloatArray> temperatureArray = vtkFloatArray::SafeDownCast(outputDataSet->GetPointData()->GetArray("temperatures"));
		ASSERT_TRUE(temperatureArray);
		
		EXPECT_FLOAT_EQ(temperaturesOfFirstPoint[t], *temperatureArray->GetTuple(0));
		EXPECT_FLOAT_EQ(temperaturesOfSecondPoint[t], *temperatureArray->GetTuple(1));
	}
	
	// Test an intermediate value
	filter->GetOutputInformation(0)->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), 0.5);
	filter->Update();

	// Run the filter on the input data
	vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
	outputDataSet->ShallowCopy(filter->GetPolyDataOutput());

	ASSERT_TRUE(outputDataSet);

	// Extract the filter's output
	vtkSmartPointer<vtkFloatArray> temperatureArray = vtkFloatArray::SafeDownCast(outputDataSet->GetPointData()->GetArray("temperatures"));
	ASSERT_TRUE(temperatureArray);
	
	EXPECT_FLOAT_EQ(22.14, *temperatureArray->GetTuple(0));
	EXPECT_FLOAT_EQ(18.8, *temperatureArray->GetTuple(1));
}