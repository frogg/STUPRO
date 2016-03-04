#include <gtest/gtest.h>

#include <Utils/Math/GeographicFunctions.hpp>
#include <Utils/Misc/Macros.hpp>

TEST(TestGeographicFunctions, TestGreatCircleDistance) {
	double acceptableError = 0.001;
	double earthRadius = 6372.8;

	// Edge case: Distance between two points on the North Pole
	EXPECT_NEAR(0.0, calculateDistance(Coordinate(90, 0), Coordinate(90, 20)), acceptableError);

	// Edge case: Distance between the North Pole and the South Pole
	EXPECT_NEAR(KRONOS_PI * earthRadius, calculateDistance(Coordinate(90, 0), Coordinate(-90, 0)),
	            acceptableError);

	// Some standard distance calculations
	EXPECT_NEAR(1359.638, calculateDistance(Coordinate(50.0, 20.0), Coordinate(40.0, 30.0)),
	            acceptableError);
	EXPECT_NEAR(15.602, calculateDistance(Coordinate(10.3, 15.2), Coordinate(10.4, 15.1)),
	            acceptableError);
	EXPECT_NEAR(157.270, calculateDistance(Coordinate(-1, 1), Coordinate(-2, 2)), acceptableError);

	// Finally, a distance calculation with a geodesic line that wraps around the latitude and longitude edges
	EXPECT_NEAR(17830.350, calculateDistance(Coordinate(80, 170), Coordinate(-80, -170)),
	            acceptableError);
}
