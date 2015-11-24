#include <Utils/Config/Configuration.hpp>

#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QStringList>
#include <QList>

#include <iostream>

const QString Configuration::TYPE_NAMES[] = {"Null", "Bool (False)", "Bool (True)", "Object", "Array", "String", "Number"};

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
  // The nodes of type Value along the path have to be stored in a list as to
  // not accidentally prune the tree of the JSON document.
  // The elements have to be pointers because rapidjson::Value cannot be passed
  // by copy and QLists do not work with references.
  QStringList path = key.split(".");
  QList<rapidjson::Value*> values;
  values.append(&this->configurationDocument);

  // Step through the data tree using the path as given by the key parameter
  for (QString step : path) {
    if (values.last()->HasMember(step.toStdString().c_str())) {
      values.append(&(*values.last())[step.toStdString().c_str()]);
    } else {
      throw InvalidKeyException(key);
    }
  }

  return *values.last();
}

const bool Configuration::hasKey(QString key) {
  try {
    this->getValueFromPath(key);
  } catch (InvalidKeyException e) {
    return false;
  }

  return true;
}

const QString Configuration::getString(QString key) {
  const rapidjson::Value &value = this->getValueFromPath(key);

  if(!value.IsString()) {
    throw InvalidValueException(key, QString("String"), Configuration::TYPE_NAMES[value.GetType()]);
  }

  return value.GetString();
}

const int Configuration::getInteger(QString key) {
  const rapidjson::Value &value = this->getValueFromPath(key);

  if(!value.IsInt()) {
    // The value is of some kind of number that is not an integer
    if(value.GetType() == 6) {
      throw InvalidValueException(key, QString("Integer"), QString("Number but not an Integer"));
    } else {
      throw InvalidValueException(key, QString("Integer"), Configuration::TYPE_NAMES[value.GetType()]);
    }
  }

  return value.GetInt();
}

const double Configuration::getDouble(QString key) {
  const rapidjson::Value &value = this->getValueFromPath(key);

  if(!value.IsDouble()) {
    // The value is of some kind of number that is not a double
    if(value.GetType() == 6) {
      throw InvalidValueException(key, QString("Double"), QString("Number but not a Double"));
    } else {
      throw InvalidValueException(key, QString("Double"), Configuration::TYPE_NAMES[value.GetType()]);
    }
  }

  return value.GetDouble();
}
