#include <qfile.h>
#include <qfileinfo.h>
#include <qiodevice.h>
#include <qlist.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include <Utils/TileDownload/ConfigUtil.hpp>

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

	while (!in.atEnd()) {
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

	return layers;
}
