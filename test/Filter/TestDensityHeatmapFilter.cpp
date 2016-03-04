#include <gtest/gtest.h>

#include <Filter/HeatmapDensityFilter.h>
#include <Reader/vtkKronosReader.h>
#include <Utils/Math/Vector3.hpp>

#include <vtkIntArray.h>
#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkPointData.h>

#include <qlist.h>

TEST(TestDensityHeatmapFilter, TestTwitterData) {
	// Read some test data
	vtkSmartPointer<vtkKronosReader> kronosReader = vtkSmartPointer<vtkKronosReader>::New();
	kronosReader->SetFileName("res/test-data/temporal-interpolation-test/twitter-test-data.kJson",
	                          false);

	// Set up the filter and its input
	vtkSmartPointer<HeatmapDensityFilter> filter = HeatmapDensityFilter::New();
	filter->SetInputConnection(kronosReader->GetOutputPort());
	filter->GetInputInformation()->Set(Data::VTK_DATA_TYPE(), Data::TWEETS);
	filter->GetInputInformation()->Set(Data::VTK_TIME_RESOLUTION(), 1);
	filter->setHorizontalHeatmapResolution(1);
	filter->setVerticalHeatmapResolution(1);
	filter->Update();

	// Test the density values for different time steps
	QList<QList<int>> densityValues = QList<QList<int>>() << (QList<int>() << 5 << 4 << 4 << 5) <<
	                                  (QList<int>() << 2 << 2 << 3 << 1) << (QList<int>() << 8 << 3 << 4 << 3) <<
	                                  (QList<int>() << 4 << 5 << 2 << 5);

	for (int t = 0; t < 3; t++) {
		// Run the filter for the time step
		filter->GetOutputInformation(0)->Set(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP(), t);
		filter->Update();

		// Extract the filter's output
		vtkSmartPointer<vtkPolyData> outputDataSet = vtkSmartPointer<vtkPolyData>::New();
		outputDataSet->ShallowCopy(vtkPolyData::SafeDownCast(filter->GetOutputDataObject(0)));

		// Do some basic sanity checks
		ASSERT_TRUE(outputDataSet);
		EXPECT_EQ(4, outputDataSet->GetNumberOfPoints());

		// Actually check if the density values are correct
		vtkSmartPointer<vtkIntArray> densityArray = vtkIntArray::SafeDownCast(
		            outputDataSet->GetPointData()->GetArray("density"));
		ASSERT_TRUE(densityArray);

		EXPECT_EQ(4, outputDataSet->GetPointData()->GetArray("density")->GetNumberOfTuples());

		for (int i = 0; i < 4; i++) {
			EXPECT_EQ(densityValues[t][i], densityArray->GetTuple1(i));
		}
	}
}
