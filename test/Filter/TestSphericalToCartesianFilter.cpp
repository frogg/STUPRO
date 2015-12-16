#include <gtest/gtest.h>
#include <Filter/SpericalToCartesianFilter.h>
#include <vtkSmartPointer.h>
#include <cmath>
#include <iostream>


TEST(TestSphericalToCartesianFilter, ValidCalls) {
    vtkSmartPointer<SpericalToCartesianFilter> filter = vtkSmartPointer<SpericalToCartesianFilter>::New();
    double points[3];
    points[0] = 0;
    points[1] = 0;
    points[2] = 0;

    
    double result[3];
    
    filter->transformToCartesian(result,200);


    EXPECT_TRUE(std::abs(result[1]) < 0.01);
    EXPECT_TRUE(std::abs(result[1]) < 0.01);
    EXPECT_TRUE(std::abs(result[0]-200.0) < 0.01);
    
    std::cout<<result[0] << ":   x" << std::endl;
    std::cout<<result[1] << ":   y" << std::endl;
    std::cout<<result[2] << ":   z" << std::endl;
    
    points[0] = 90;
    points[1] = 0;
    points[2] = 0;
    filter->transformToCartesian(result,200);

    std::cout<<result[0] << ":   x" << std::endl;
    std::cout<<result[1] << ":   y" << std::endl;
    std::cout<<result[2] << ":   z" << std::endl;

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
