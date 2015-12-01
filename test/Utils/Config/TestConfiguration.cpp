#include <gtest/gtest.h>
#include <QString>
#include <string>
#include <Utils/Config/Configuration.hpp>

TEST(TestConfiguration, ValidCalls) {
	EXPECT_TRUE(Configuration::getInstance().hasKey("globe.radius"));
	EXPECT_TRUE(!Configuration::getInstance().hasKey("globe.invalid"));

	EXPECT_EQ(
		42,
		Configuration::getInstance().getInteger("globe.radius")
	);

	EXPECT_EQ(
		std::string("Foo"),
		Configuration::getInstance().getString("globe.name").toStdString()
	);

	EXPECT_EQ(
		true,
		Configuration::getInstance().getBoolean("globe.spheric")
	);

	EXPECT_EQ(
		12.2345,
		Configuration::getInstance().getDouble("latitude")
	);

	EXPECT_EQ(
		12.2345f,
		Configuration::getInstance().getFloat("latitude")
	);

	EXPECT_EQ(
		5.234,
		Configuration::getInstance().getDouble("deeply.nested.configuration.group.longitude")
	);
}

TEST(TestConfiguration, InvalidCalls) {
	EXPECT_THROW(
		Configuration::getInstance().getString("globe.invalid"),
		InvalidKeyException
	);

	EXPECT_THROW(
		Configuration::getInstance().getDouble("globe.name"),
		InvalidValueException
	);
}
