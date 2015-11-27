#include "TestConfiguration.hpp"

#include <cppunit/TestAssert.h>
#include <QString>
#include <string>
#include <Utils/Config/Configuration.hpp>

void TestConfiguration::testValidCalls() {
	CPPUNIT_ASSERT(Configuration::getInstance().hasKey("globe.radius"));
	CPPUNIT_ASSERT(!Configuration::getInstance().hasKey("globe.invalid"));

	CPPUNIT_ASSERT_EQUAL(
		42,
		Configuration::getInstance().getInteger("globe.radius")
	);

	CPPUNIT_ASSERT_EQUAL(
		std::string("Foo"),
		Configuration::getInstance().getString("globe.name").toStdString()
	);

	CPPUNIT_ASSERT_EQUAL(
		true,
		Configuration::getInstance().getBoolean("globe.spheric")
	);

	CPPUNIT_ASSERT_EQUAL(
		12.2345,
		Configuration::getInstance().getDouble("latitude")
	);

	CPPUNIT_ASSERT_EQUAL(
		12.2345f,
		Configuration::getInstance().getFloat("latitude")
	);

	CPPUNIT_ASSERT_EQUAL(
		5.234,
		Configuration::getInstance().getDouble("deeply.nested.configuration.group.longitude")
	);
}

void TestConfiguration::testInvalidCalls() {
	CPPUNIT_ASSERT_THROW(
		Configuration::getInstance().getString("globe.invalid"),
		InvalidKeyException
	);

	CPPUNIT_ASSERT_THROW(
		Configuration::getInstance().getDouble("globe.name"),
		InvalidValueException
	);
}
