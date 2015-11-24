#include <Utils/TileDownload/ConfigUtil.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <QFile>
#include <QList>
#include <QFileInfo>
#include <QTextStream>

#include <iostream>

using namespace rapidjson;

/* Initialize constant strings that contain potential exception messages */
const QString ConfigUtil::FILE_DOES_NOT_EXIST_MESSAGE = QString("The specified config file"
							" at '%1' could not be opened since it does not seem to exist.");
const QString ConfigUtil::FILE_COULD_NOT_BE_OPENED_MESSAGE = QString("The specified config file"
							" at '%1' could not be opened: %2");
const QString ConfigUtil::FILE_COULD_NOT_BE_PARSED_MESSAGE = QString("The specified config file"
							" at '%1' could not be parsed: %2");

const QMap<QString, ImageLayerDescription> ConfigUtil::loadConfigFile(const QString &file) {
	QMap<QString, ImageLayerDescription> layers;

	QFile configFile(file);
	QFileInfo configFileInfo(file);

	if (!configFile.exists()) {
		throw FileOpenException(ConfigUtil::FILE_DOES_NOT_EXIST_MESSAGE
															.arg(configFileInfo.absoluteFilePath()));
	}

	if (!configFile.open(QIODevice::ReadOnly)) {
		throw FileOpenException(ConfigUtil::FILE_COULD_NOT_BE_OPENED_MESSAGE
															.arg(configFileInfo.absoluteFilePath())
															.arg(configFile.errorString()));
	}

	QTextStream in(&configFile);
	QString configText = in.readAll();
	configFile.close();

	Document configDocument;
	configDocument.Parse(configText.toStdString().c_str());

	if (configDocument.HasParseError()) {
		throw FileOpenException(ConfigUtil::FILE_COULD_NOT_BE_PARSED_MESSAGE
															.arg(configFileInfo.absoluteFilePath())
															.arg(GetParseError_En(configDocument.GetParseError())));
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
