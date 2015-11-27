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

Configuration &Configuration::getInstance() {
	static Configuration instance;
	instance.loadConfigurationFile();
	return instance;
}

Configuration::Configuration() : CONFIGURATION_FILE_PATH(QString("./res/configuration.json")) { }

void Configuration::loadConfigurationFile() {
	// Open the configuration file while checking for potential errors
	QFile configFile(this->CONFIGURATION_FILE_PATH);
  QFileInfo configFileInfo(this->CONFIGURATION_FILE_PATH);

  if (!configFile.open(QIODevice::ReadOnly)) {
    throw ConfigFileOpenException(configFileInfo.absoluteFilePath(), configFile.errorString());
  }

	// Read from the opened stream and save the file's contents in a string
  QTextStream in(&configFile);
  QString configText = in.readAll();
  configFile.close();

	// Parse the file's content and construct a representation inside the memory
	// using a `rapidjson::Document`
	rapidjson::Document configurationDocument;
  configurationDocument.Parse(configText.toStdString().c_str());

  if (configurationDocument.HasParseError()) {
    throw JsonParseException(
      configFileInfo.absoluteFilePath(),
      rapidjson::GetParseError_En(configurationDocument.GetParseError())
    );
  }

	// Start recursively iterating the JSON tree and save all primitive values to the map.
	// The actual JSON document can go out of scope afterwards.
	this->indexValues(configurationDocument, QString(""));
}

void Configuration::indexValues(rapidjson::Value& jsonValue, QString path) {
	// Iterate through each direct descendant of the JSON object `jsonValue`
	for (rapidjson::Value::ConstMemberIterator iterator = jsonValue.MemberBegin();
				iterator != jsonValue.MemberEnd(); ++iterator) {
		if (iterator->value.GetType() == 3) {
			// The descendant is of type 3 which is RapidJSON's way of marking it as
			// a complex nested object
			QString newPath;
			if (path.length() == 0) {
				newPath = QString(iterator->name.GetString());
			} else {
				newPath = path + "." + QString(iterator->name.GetString());
			}

			// Recursively iterate through the nested object using the previously
			// constructed path
			this->indexValues(jsonValue[iterator->name.GetString()], newPath);
		} else {
			// The descendant is not of type 3 and therefore some kind of primitive
			// that can be directly written to the map
			QString valuePath = path;
			if (valuePath.length() == 0) {
				valuePath = QString(iterator->name.GetString());
			} else {
				valuePath += "." + QString(iterator->name.GetString());
			}

			// Recognize the type of the primitive value and wrap it in a `ConfigurationValue`
			ConfigurationValue primitiveValue;
			if (iterator->value.IsString()) {
				primitiveValue = ConfigurationValue(QString(iterator->value.GetString()));
			} else if (iterator->value.IsBool()) {
				primitiveValue = ConfigurationValue(iterator->value.GetBool());
			} else if (iterator->value.IsInt()) {
				primitiveValue = ConfigurationValue(iterator->value.GetInt());
			} else if (iterator->value.IsDouble()) {
				primitiveValue = ConfigurationValue(iterator->value.GetDouble());
			} else {
				throw UnsupportedValueException(valuePath);
			}

			// Finally insert the primitive value into the map
			this->values.insert(valuePath, primitiveValue);
		}
	}
}

bool Configuration::hasKey(QString key) const {
  return this->values.contains(key);
}

ConfigurationValue Configuration::getConfigurationValue(QString key, int type) const {
	if (!this->hasKey(key)) {
		throw InvalidKeyException(key);
	} else {
		ConfigurationValue foundValue = this->values.value(key);
		if (foundValue.getType() == type) {
			return foundValue;
		} else {
			throw InvalidValueException(
				key, ConfigurationValue::getTypeNameFromInteger(type), foundValue.getTypeName()
			);
		}
	}
}

QString Configuration::getString(QString key) const {
	return this->getConfigurationValue(key, ConfigurationValue::TYPE_STRING).getStringValue();
}

bool Configuration::getBoolean(QString key) const {
	return this->getConfigurationValue(key, ConfigurationValue::TYPE_BOOLEAN).getBooleanValue();
}

int Configuration::getInteger(QString key) const {
	return this->getConfigurationValue(key, ConfigurationValue::TYPE_INTEGER).getIntegerValue();
}

double Configuration::getDouble(QString key) const {
	return this->getConfigurationValue(key, ConfigurationValue::TYPE_DOUBLE).getDoubleValue();
}

float Configuration::getFloat(QString key) const {
	return (float) this->getConfigurationValue(key, ConfigurationValue::TYPE_DOUBLE)
			.getDoubleValue();
}
