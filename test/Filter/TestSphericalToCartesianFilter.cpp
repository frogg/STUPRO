#include <gtest/gtest.h>
#include <Filter/SphericalToCartesianFilter.h>
#include <vtkSmartPointer.h>
#include <cmath>
#include <iostream>


// write further tests with http://www.random-science-tools.com/maths/coordinate-converter.htm
TEST(TestSphericalToCartesianFilter, ValidCalls) {
    vtkSmartPointer<SphericalToCartesianFilter> filter = vtkSmartPointer<SphericalToCartesianFilter>::New();
    double points[3];
    points[0] = 0;
    points[1] = 0;
    points[2] = 0;
    
    filter->transformToCartesian(points,200);

    EXPECT_TRUE(std::abs(points[0]) < 0.01);
    EXPECT_TRUE(std::abs(points[1]) < 0.01);
    EXPECT_TRUE(std::abs(points[2]-200.0) < 0.01);
    
    points[0] = 90;
    points[1] = 0;
    points[2] = 0;
    filter->transformToCartesian(points,300);

    EXPECT_TRUE(std::abs(points[0]-300.0) < 0.01);
    EXPECT_TRUE(std::abs(points[1]) < 0.01);
    EXPECT_TRUE(std::abs(points[2]) < 0.01);

    points[0] = 0;
    points[1] = 90;
    points[2] = 10;
    filter->transformToCartesian(points,100);
    
    EXPECT_TRUE(std::abs(points[0]) < 0.01);
    EXPECT_TRUE(std::abs(points[1]-110.0) < 0.01);
    EXPECT_TRUE(std::abs(points[2]) < 0.01);
    
    //negative angles
    points[0] = -90;
    points[1] = 0;
    points[2] = 0;
    filter->transformToCartesian(points,3);
    EXPECT_TRUE(std::abs(points[0]+3) < 0.01);
    EXPECT_TRUE(std::abs(points[1]) < 0.01);
    EXPECT_TRUE(std::abs(points[2]) < 0.01);

    /*
    points[0] = 45;
    points[1] = 45;
    points[2] = 0;
    filter->transformToCartesian(points,30);

    EXPECT_TRUE(std::abs(points[0]-15) < 0.01);
    EXPECT_TRUE(std::abs(points[1]-15) < 0.01);
    EXPECT_TRUE(std::abs(points[2]-21.21) < 0.1);
    
    
    std::cout<<points[0] << ":   x" << std::endl;
    std::cout<<points[1] << ":   y" << std::endl;
    std::cout<<points[2] << ":   z" << std::endl;

    points[0] = 120;
    points[1] = 60;
    points[2] = 30;
    filter->transformToCartesian(points,0);
    
    EXPECT_TRUE(std::abs(points[0]+64.95) < 0.01);
    EXPECT_TRUE(std::abs(points[1]-112.5) < 0.01);
    EXPECT_TRUE(std::abs(points[2]-75.00) < 0.1);


    
    std::cout<<points[0] << ":   x" << std::endl;
    std::cout<<points[1] << ":   y" << std::endl;
    std::cout<<points[2] << ":   z" << std::endl;
    */




//    EXPECT_TRUE(std::abs(result[0]-100) < 0.01);
 //   EXPECT_TRUE(std::abs(result[0]-100) < 0.01);

/*
    EXPECT_EQ(
              42,
              Configuration::getInstance().getInteger("globe.radius")
              );
    
    EXPECT_EQ(
              12.2345,
              Configuration::getInstance().getDouble("latitude")
              );
    EXPECT_EQ(
              5.234,
              Configuration::getInstance().getDouble("deeply.nested.configuration.group.longitude")
              );
*/
}

TEST(SphericalToCartesianFilter, InvalidCalls) {
  /*  double points[3];
    points[0] = -135;
    points[1] = 125.264;
    points[2] = 0;
    filter->transformToCartesian(points,51.9615);
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
