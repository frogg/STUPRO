#include <gtest/gtest.h>
#include <Filter/SpericalToCartesianFilter.h>
#include <vtkSmartPointer.h>


TEST(SpericalToCartesianFilter, ValidCalls) {
    vtkSmartPointer<SpericalToCartesianFilter> filter = vtkSmartPointer<SpericalToCartesianFilter>::New();
    double points[2];
    points[0] = 0;
    points[1] = 0;
    
    double result[3];
    
    filter->transformToCartesian(result,100);
    std::cout<<result[0] << "test";
    std::cout<<result[1] << "test";
    std::cout<<result[2] << "test";

    /*
    vtkPoints *points = output->GetPoints();
    //get the point of the output and transform them to cartesian coordinate system
    for (int i = 0; i < points->GetNumberOfPoints(); i++)
    {
        points->SetPoint(i, transformToCartesian(points->GetPoint(i)));
    }
    */
    
   /* EXPECT_TRUE(Configuration::getInstance().hasKey("globe.radius"));
    EXPECT_TRUE(!Configuration::getInstance().hasKey("globe.invalid"));
    
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

TEST(SpericalToCartesianFilter, InvalidCalls) {
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
