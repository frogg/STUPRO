#include <Utils/TileDownload/ConfigUtil.hpp>

#include <rapidjson/document.h>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

using namespace rapidjson;

const QMap<QString, ImageLayerDescription> ConfigUtil::loadConfigFile(const QString &file) {
	QMap<QString, ImageLayerDescription> layers;

	QFile configFile(file);
	QFileInfo configFileInfo(file);

	if (!configFile.exists()) {
		throw FileOpenException("The specified config file at '" + configFileInfo.absoluteFilePath() + "'"
														" could not be opened since it does not seem to exist.");
	}

	if (!configFile.open(QIODevice::ReadOnly)) {
		throw FileOpenException("The specified config file at '" + configFileInfo.absoluteFilePath() + "'"
														" could not be opened: " + configFile.errorString());
	}

	QTextStream in(&configFile);
	QString configText = in.readAll();
	configFile.close();

	Document configDocument;
	configDocument.Parse(configText.toStdString().c_str());

	for (Value::ConstMemberIterator layerIterator = configDocument.MemberBegin();
	    layerIterator != configDocument.MemberEnd(); ++layerIterator) {
	    printf("Found member %s with zoom levels %s\n", layerIterator->name.GetString(), layerIterator->value["zoomLevels"].MemberBegin()->name.GetString());
	}

	/*while (!in.atEnd()) {
		QString line = in.readLine();
		QStringList fields = line.split(' ');

		if (fields.size() > 0) {
			if (fields.size() < 4 || fields.at(0).startsWith('#')) {
				continue;
			}
		} else {
			continue;
		}

		ImageLayerDescription description = {fields.at(1), fields.at(2), fields.at(3).toInt()};
		layers.insert(fields.at(0), description);
	}

	configFile.close();

	return layers;*/
}
