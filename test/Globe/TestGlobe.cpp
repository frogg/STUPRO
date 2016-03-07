#include <gtest/gtest.h>
#include <gtest/gtest-message.h>
#include <gtest/gtest-test-part.h>
#include <Globe/GlobeTile.hpp>
#include <Utils/Math/Rect.hpp>

/**
 * Compare two float rectangles for near-equivalency.
 */
#define EXPECT_EQ_RECT(rectA, rectB) \
	EXPECT_FLOAT_EQ(rectA.x, rectB.x); \
	EXPECT_FLOAT_EQ(rectA.y, rectB.y); \
	EXPECT_FLOAT_EQ(rectA.w, rectB.w); \
	EXPECT_FLOAT_EQ(rectA.h, rectB.h);

TEST(TestGlobe, tileLocation) {
	// Left half of the globe.
	GlobeTile::Location loc000(0, 0, 0);

	// Right half of the globe.
	GlobeTile::Location loc010(0, 1, 0);

	// Test constructor argument order (zoom, lon, lat).
	GlobeTile::Location loc123(1, 2, 3);
	EXPECT_EQ(loc123.zoomLevel, 1);
	EXPECT_EQ(loc123.longitude, 2);
	EXPECT_EQ(loc123.latitude, 3);

	// Test outermost zoom level rectangle bounds. Y-coordinate should be the same.
	RectF rect000 = loc000.getBounds();
	RectF rect000Flipped = loc000.getFlippedBounds();
	EXPECT_EQ_RECT(rect000, rect000Flipped);

	// Test inner tile at x=2 y=3 with zoomLevel 3.
	// The globe should have a width of 2^4 = 16 Tiles and a height of 2^3 = 8 Tiles.
	// This means each tile is 180°/8 = 22.5° wide and tall.
	// Therefore, the tile should have the position (2 * 22.5 - 180 = -135, 90 - (3 + 1) * 22.5 = 0) and size (22.5, 22.5).
	// The adjustment by 1 tile in the Y axis is necessary because of the upside-down coordinate system.
	GlobeTile::Location loc323(3, 2, 3);
	RectF rect323 = loc323.getBounds();
	EXPECT_EQ_RECT(RectF(-135, 0, 22.5, 22.5), rect323);
}
