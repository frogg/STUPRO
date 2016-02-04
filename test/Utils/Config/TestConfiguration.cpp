#include <gtest/gtest.h>
#include <QString>
#include <string>
#include <Utils/Config/Configuration.hpp>

TEST(TestConfiguration, ValidCalls) {
	EXPECT_TRUE(Configuration::getInstance().hasKey("globe.radius"));
	EXPECT_TRUE(!Configuration::getInstance().hasKey("globe.invalid"));

	EXPECT_EQ(
	    100.0,
	    Configuration::getInstance().getDouble("globe.radius")
	);
}

TEST(TestConfiguration, InvalidCalls) {
	EXPECT_THROW(
	    Configuration::getInstance().getString("globe.invalid"),
	    InvalidKeyException
	);

	EXPECT_THROW(
	    Configuration::getInstance().getInteger("globe.radius"),
	    InvalidValueException
	);
}
