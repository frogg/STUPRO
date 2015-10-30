#include "TestJsonUtil.hpp"

#include <QDebug>

void TestJsonUtil::testLoadJson()
{
	QJsonObject jsonObject = JsonUtil::loadJsonFile("../../res/config.json");
	std::cout << "TEST OUTPUT" << std::endl;

	CPPUNIT_ASSERT(jsonObject.value("tileSize").toInt() == 512);
}
