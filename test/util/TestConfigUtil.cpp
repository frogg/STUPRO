#include "TestConfigUtil.hpp"

#include <ImageLayerDescription.hpp>
#include <QDebug>
#include <QString>
#include <QMap>

void TestConfigUtil::testLoadConfig()
{
	QMap<QString, ImageLayerDescription> layers;

	try {
		layers = ConfigUtil::loadConfigFile("./src/res/layers.cfg");
	} catch(FileOpenException e) {
		std::cout << "Could not open the file: " << e.what() << std::endl;
	}

	std::cout << layers.value("satellite-imagery").baseUrl.toUtf8().constData() << std::endl;

	CPPUNIT_ASSERT(512 == 511);
}
