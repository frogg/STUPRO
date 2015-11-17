#include <Utils/TileDownload/ConfigUtil.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <QFile>
#include <QList>
#include <QFileInfo>
#include <QTextStream>

#include <iostream>

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

	if (configDocument.HasParseError()) {
		throw FileOpenException("The specified config file at '" + configFileInfo.absoluteFilePath() + "'"
														" could not be parsed: " + GetParseError_En(configDocument.GetParseError()));
	}

	for (Value::ConstMemberIterator layerIterator = configDocument.MemberBegin();
	    layerIterator != configDocument.MemberEnd(); ++layerIterator) {
		QList<LayerStep> layerSteps;

		const Value &zoomLevels = layerIterator->value["zoomLevels"];
		for (SizeType i = 0; i < zoomLevels.Size(); i++) {
			LayerStep layerStep = {
				zoomLevels[i]["minimalZoomLevel"].GetInt(),
				zoomLevels[i]["layers"].GetString()
			};
			layerSteps.append(layerStep);
		}

		ImageLayerDescription layerDescription(
			layerIterator->value["baseUrl"].GetString(),
			layerIterator->value["mimeType"].GetString(),
			layerIterator->value["tileSize"].GetInt(),
			layerSteps
		);

		layers.insert(layerIterator->name.GetString(), layerDescription);
	}

	return layers;
}
