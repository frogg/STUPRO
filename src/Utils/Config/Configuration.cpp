#include <Utils/Config/Configuration.hpp>

#include <rapidjson/error/en.h>

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

Configuration &Configuration::getInstance() {
	static Configuration instance;
	return instance;
}

Configuration::Configuration() : CONFIGURATION_FILE_PATH(QString("./res/configuration.json")) {
  QFile configFile(this->CONFIGURATION_FILE_PATH);
  QFileInfo configFileInfo(this->CONFIGURATION_FILE_PATH);

  if (!configFile.open(QIODevice::ReadOnly)) {
    throw ConfigFileOpenException(configFileInfo.absoluteFilePath(), configFile.errorString());
  }

  QTextStream in(&configFile);
  QString configText = in.readAll();
  configFile.close();

  this->configurationDocument.Parse(configText.toStdString().c_str());

  if (this->configurationDocument.HasParseError()) {
    throw JsonParseException(
      configFileInfo.absoluteFilePath(),
      rapidjson::GetParseError_En(this->configurationDocument.GetParseError())
    );
  }
}

const bool Configuration::hasKey(QString key) {
  // TODO
}

const QString Configuration::getString(QString key) {
  // TODO
}

const int Configuration::getInteger(QString key) {
  // TODO
}

const float Configuration::getFloat(QString key) {
  // TODO
}

const double Configuration::getDouble(QString key) {
  // TODO
}
