#include <Utils/Config/ConfigurationValue.hpp>

// Has to use the constructor + <<-operator instead of an initializer-list
const QList<QString> ConfigurationValue::TYPE_NAMES = QList<QString>() << "Unknown" << "String" <<
        "Boolean" << "Integer" << "Double";

const QString ConfigurationValue::getTypeNameFromInteger(int type) {
	if (type >= 0 && type < ConfigurationValue::TYPE_NAMES.size()) {
		return ConfigurationValue::TYPE_NAMES.at(type);
	} else {
		return ConfigurationValue::TYPE_NAMES.at(0);
	}
}

const int ConfigurationValue::getType() {
	return this->valueType;
}

const QString ConfigurationValue::getTypeName() {
	return ConfigurationValue::getTypeNameFromInteger(this->getType());
}

const QString ConfigurationValue::getStringValue() {
	return this->stringValue;
}

const bool ConfigurationValue::getBooleanValue() {
	return this->booleanValue;
}

const int ConfigurationValue::getIntegerValue() {
	return this->integerValue;
}

const double ConfigurationValue::getDoubleValue() {
	return this->doubleValue;
}
