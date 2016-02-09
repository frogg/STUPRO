#include <gtest/gtest.h>

#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataArray.h>
#include <vtkTypeInt32Array.h>
#include <vtkPointData.h>

#include <Filter/PrecipitationTypeFilter.h>
#include <Reader/DataReader/JsonReader.hpp>
#include <Reader/DataReader/JsonReaderFactory.hpp>
#include <Reader/DataReader/DataPoints/TemporalDataPoints/PrecipitationDataPoint.hpp>
#include <Reader/DataReader/Data.hpp>

#include <QList>

void testDataSet(vtkSmartPointer<vtkUnstructuredGrid> dataSet, int expectedPointAmount,
                 QList<PrecipitationDataPoint::PrecipitationType> disallowedPrecipitationTypes) {
	// Check the data set and its amount of points
	ASSERT_TRUE(dataSet);
	EXPECT_EQ(expectedPointAmount, dataSet->GetNumberOfPoints());

	// Extract the array of precipitation types and check its type and size
	vtkSmartPointer<vtkDataArray> abstractPrecipitationTypeArray = dataSet->GetPointData()
	        ->GetArray("precipitationTypes");
	vtkSmartPointer<vtkTypeInt32Array> precipitationTypeArray = vtkTypeInt32Array::SafeDownCast(
	            abstractPrecipitationTypeArray);
	ASSERT_TRUE(precipitationTypeArray);
	ASSERT_EQ(expectedPointAmount, precipitationTypeArray->GetNumberOfTuples());

	// Ensure that no point has a precipitation type that is in the list of disallowed types
	for (int i = 0; i < expectedPointAmount; i++) {
		QList<PrecipitationDataPoint::PrecipitationType>::iterator type;
		for (type = disallowedPrecipitationTypes.begin(); type != disallowedPrecipitationTypes.end();
		        ++type) {
			EXPECT_NE(*type, precipitationTypeArray->GetTuple1(i));
		}
	}
}

TEST(TestPrecipitationTypeFilter, TestFilter) {
	// Read some test data that is useful for testing precipitation types
	std::unique_ptr<JsonReader> jsonReader =
	    JsonReaderFactory::createReader("res/test-data/precipitation-types.kJson");
	vtkSmartPointer<vtkPolyData> inputDataSet = jsonReader->getVtkDataSet(0);

	int totalNumberOfPoints = 400;
	EXPECT_EQ(totalNumberOfPoints, inputDataSet->GetNumberOfPoints());

	// Set up the filter and its input
	vtkSmartPointer<PrecipitationTypeFilter> filter = PrecipitationTypeFilter::New();
	filter->SetInputData(0, inputDataSet);
	filter->GetInputInformation()->Set(Data::VTK_DATA_TYPE(), Data::PRECIPITATION);
	filter->Update();

	// Test the filter's output without excluding any precipitation type
	vtkSmartPointer<vtkUnstructuredGrid> outputDataSet = vtkSmartPointer<vtkUnstructuredGrid>::New();
	outputDataSet->ShallowCopy(filter->GetOutput());
	testDataSet(outputDataSet, totalNumberOfPoints, QList<PrecipitationDataPoint::PrecipitationType>());

	// Exclude data points without precipitation type information
	filter->enableUndefined(0);
	filter->Update();
	outputDataSet->ShallowCopy(filter->GetOutput());
	testDataSet(outputDataSet, totalNumberOfPoints - (80 * 1),
	            QList<PrecipitationDataPoint::PrecipitationType>() << PrecipitationDataPoint::NONE);

	// Additionally, exclude data points with the precipitation type rain
	filter->enableRain(0);
	filter->Update();
	outputDataSet->ShallowCopy(filter->GetOutput());
	testDataSet(outputDataSet, totalNumberOfPoints - (80 * 2),
	            QList<PrecipitationDataPoint::PrecipitationType>() << PrecipitationDataPoint::NONE <<
	            PrecipitationDataPoint::RAIN);

	// Additionally, exclude data points with the precipitation type snow
	filter->enableSnow(0);
	filter->Update();
	outputDataSet->ShallowCopy(filter->GetOutput());
	testDataSet(outputDataSet, totalNumberOfPoints - (80 * 3),
	            QList<PrecipitationDataPoint::PrecipitationType>() << PrecipitationDataPoint::NONE <<
	            PrecipitationDataPoint::RAIN << PrecipitationDataPoint::SNOW);

	// Additionally, exclude data points with the precipitation type sleet
	filter->enableSleet(0);
	filter->Update();
	outputDataSet->ShallowCopy(filter->GetOutput());
	testDataSet(outputDataSet, totalNumberOfPoints - (80 * 4),
	            QList<PrecipitationDataPoint::PrecipitationType>() << PrecipitationDataPoint::NONE <<
	            PrecipitationDataPoint::RAIN << PrecipitationDataPoint::SNOW << PrecipitationDataPoint::SLEET);

	// Additionally, exclude data points with the precipitation type hail, therefore eliminating all data points
	filter->enableHail(0);
	filter->Update();
	outputDataSet->ShallowCopy(filter->GetOutput());
	testDataSet(outputDataSet, 0, QList<PrecipitationDataPoint::PrecipitationType>() <<
	            PrecipitationDataPoint::NONE << PrecipitationDataPoint::RAIN << PrecipitationDataPoint::SNOW <<
	            PrecipitationDataPoint::SLEET << PrecipitationDataPoint::HAIL);

	// Finally, enable everything again, which should give us back all points we started with in the first place
	filter->enableUndefined(1);
	filter->enableRain(1);
	filter->enableSnow(1);
	filter->enableSleet(1);
	filter->enableHail(1);
	filter->Update();
	outputDataSet->ShallowCopy(filter->GetOutput());
	testDataSet(outputDataSet, totalNumberOfPoints, QList<PrecipitationDataPoint::PrecipitationType>());
}