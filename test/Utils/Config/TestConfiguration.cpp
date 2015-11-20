#include "TestConfiguration.hpp"

#include <cppunit/TestAssert.h>
#include <QString>
#include <string>
#include <Utils/Config/Configuration.hpp>

void TestConfiguration::testValidCalls() {
	CPPUNIT_ASSERT(Configuration::getInstance().hasKey("globe.radius"));
	CPPUNIT_ASSERT(!Configuration::getInstance().hasKey("globe.invalid"));

	CPPUNIT_ASSERT_EQUAL(
		Configuration::getInstance().getInteger("globe.radius"),
		42
	);

	CPPUNIT_ASSERT_EQUAL(
		Configuration::getInstance().getString("globe.name").toStdString(),
		std::string("Foo")
	);

	CPPUNIT_ASSERT_EQUAL(
		Configuration::getInstance().getFloat("latitude"),
		12.2345
	);

	CPPUNIT_ASSERT_EQUAL(
		Configuration::getInstance().getDouble("deeply.nested.configuration.group.longitude"),
		5.234f
	);
}

void TestConfiguration::testInvalidCalls() {
	CPPUNIT_ASSERT_THROW(
		Configuration::getInstance().getString("globe.invalid"),
		InvalidKeyException
	);

	CPPUNIT_ASSERT_THROW(
		Configuration::getInstance().getFloat("globe.name"),
		InvalidValueException
	);
}
