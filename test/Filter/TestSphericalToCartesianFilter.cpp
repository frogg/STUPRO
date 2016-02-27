#include <gtest/gtest.h>
#include "Filter/GeometryTransform.hpp"
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <cmath>
#include <iostream>
#include <QString>

#define CHECK_POINT(POINT,VALUE,PRECISION){\
		if(std::abs((POINT)[0]-(VALUE)[0]) < PRECISION) GTEST_SUCCEED(); \
		else GTEST_MESSAGE_(QString("FAILURE X: Exp: %1, Actual: %2").arg(POINT_TO_STRING(POINT)).arg(POINT_TO_STRING(VALUE)).toStdString().c_str(),\
			                    ::testing::TestPartResult::kFatalFailure);\
		if(std::abs((POINT)[1]-(VALUE)[1]) < PRECISION) GTEST_SUCCEED(); \
		else GTEST_MESSAGE_(QString("FAILURE Y: Exp: %1, Actual: %2").arg(POINT_TO_STRING(POINT)).arg(POINT_TO_STRING(VALUE)).toStdString().c_str(),\
			                    ::testing::TestPartResult::kFatalFailure);\
		if(std::abs((POINT)[2]-(VALUE)[2]) < PRECISION) GTEST_SUCCEED(); \
		else GTEST_MESSAGE_(QString("FAILURE X: Exp: %1, Actual: %2").arg(POINT_TO_STRING(POINT)).arg(POINT_TO_STRING(VALUE)).toStdString().c_str(),\
			                    ::testing::TestPartResult::kFatalFailure);}

#define POINT_TO_STRING(POINT)\
	QString("( %1 / %2 / %3 )").arg((POINT)[0]).arg((POINT)[1]).arg((POINT)[2])


// write further tests with http://www.random-science-tools.com/maths/coordinate-converter.htm
TEST(TestSphericalToCartesianFilter, ValidCalls) {
	GeometryTransform* transform = new GeometryTransform(true, true);
	vtkPoints* points = vtkPoints::New();
	vtkPoints* transformed = vtkPoints::New();
	vtkPoints* expectedPointsAfterTransformation = vtkPoints::New();
	const double precision = 0.01;

	points->InsertNextPoint(0, 0, 100);
	expectedPointsAfterTransformation->InsertNextPoint(0, 0, 200);
	points->InsertNextPoint(90, 0, 100);
	expectedPointsAfterTransformation->InsertNextPoint(200, 0, 0);
	points->InsertNextPoint(0, 90, 10);
	expectedPointsAfterTransformation->InsertNextPoint(0, 110, 0);
	points->InsertNextPoint(-90, 0, 100);
	expectedPointsAfterTransformation->InsertNextPoint(-200, 0, 0);

	transformed->Allocate(points->GetNumberOfPoints());
	transform->TransformPoints(points, transformed);

	for (int i = 0; i < points->GetNumberOfPoints() && i < expectedPointsAfterTransformation->GetNumberOfPoints(); i++) {
		CHECK_POINT(expectedPointsAfterTransformation->GetPoint(i), transformed->GetPoint(i), precision);
	}
}
