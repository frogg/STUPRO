#include <gtest/gtest.h>

#include <Utils/Math/SphericalCoordinateFunctions.h>
#include <Utils/Misc/Macros.hpp>

#define EXPECT_EQ_TOLERANCE(X1, X2, TOL) { \
		EXPECT_LE(X1, (X2) + TOL); \
		EXPECT_GE(X1, (X2) - TOL); \
	}

#define EXPECT_V_EQ(V1, V2) {\
		EXPECT_EQ_TOLERANCE((V1).x, (V2).x, 1e-5);\
		EXPECT_EQ_TOLERANCE((V1).y, (V2).y, 1e-5);\
		EXPECT_EQ_TOLERANCE((V1).z, (V2).z, 1e-5);\
	}

TEST(TestSphericalCoordinateFunctions, TestInverseTransform) {
	Vector3d testV(90, 23, 100);
	EXPECT_V_EQ(testV, cartesianToSpherical(sphericalToCartesian(testV)));
}

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