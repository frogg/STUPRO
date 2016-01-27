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
	GeometryTransform* transform = new GeometryTransform(true, true, 200);
	vtkPoints* points = vtkPoints::New();
	vtkPoints* transformed = vtkPoints::New();
	vtkPoints* newPoints = vtkPoints::New();
	const double precision = 0.01;

	points->InsertNextPoint(0, 0, 0);
	newPoints->InsertNextPoint(0, 0, 200);
	points->InsertNextPoint(90, 0, 0);
	newPoints->InsertNextPoint(200, 0, 0);
	points->InsertNextPoint(0, 90, 10);
	newPoints->InsertNextPoint(0, 210, 0);
	points->InsertNextPoint(-90, 0, 0);
	newPoints->InsertNextPoint(-200, 0, 0);

	transformed->Allocate(points->GetNumberOfPoints());
	transform->TransformPoints(points, transformed);

	for (int i = 0; i < points->GetNumberOfPoints() && i < newPoints->GetNumberOfPoints(); i++) {
		CHECK_POINT(newPoints->GetPoint(i), transformed->GetPoint(i), precision);
	}
}

TEST(SphericalToCartesianFilter, InvalidCalls) {
	/*  double points[3];
	  points[0] = -135;
	  points[1] = 125.264;
	  points[2] = 0;
	  transform->transformToCartesian(points,51.9615);
	  */
	/*
	EXPECT_THROW(
	             Configuration::getInstance().getString("globe.invalid"),
	             InvalidKeyException
	             );

	EXPECT_THROW(
	             Configuration::getInstance().getDouble("globe.name"),
	             InvalidValueException
	             );
	 */
}
