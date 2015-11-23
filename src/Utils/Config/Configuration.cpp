#include <Utils/Config/Configuration.hpp>

#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QStringList>

#include <iostream>

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

const rapidjson::Value& Configuration::getValueFromPath(QString key) {
  // Perform a deep copy of the configuration document so that the search
  // operation does not accidentally prune the document tree.
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  this->configurationDocument.Accept(writer);
  rapidjson::Document deepCopy;
  deepCopy.Parse(buffer.GetString());
  rapidjson::Value &currentValue = deepCopy;

  QStringList path = key.split(".");

  // Step through the data tree using the path as given by the key parameter
  for (QString step : path) {
    if (currentValue.HasMember(step.toStdString().c_str())) {
      currentValue = currentValue[step.toStdString().c_str()];
    } else {
      throw InvalidKeyException(key);
    }
  }

  return currentValue;
}

const bool Configuration::hasKey(QString key) {

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
