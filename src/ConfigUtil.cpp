#include "../include/ConfigUtil.hpp"

#include <QFile>
#include <QIODevice>
#include <QStringList>
#include <QRegExp>
#include <QTextStream>

#include <iostream>

const QMap<QString, ImageLayerDescription> ConfigUtil::loadConfigFile(const QString &file)
{
	QMap<QString, ImageLayerDescription> layers;

	QFile configFile(file);
	if(!configFile.open(QIODevice::ReadOnly)) {
			throw FileOpenException(configFile.errorString());
	}

	QTextStream in(&configFile);

	while(!in.atEnd()) {
    QString line = in.readLine();
    QStringList fields = line.split(' ');
		ImageLayerDescription description = {fields.at(1), fields.at(2), fields.at(3).toInt()};
		layers.insert(fields.at(0), description);
	}

	configFile.close();

  return layers;
}
