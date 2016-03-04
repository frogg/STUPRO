#include <gtest/gtest.h>
#include "Utils/Math/SphericalCoordinateFunctions.h"

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
