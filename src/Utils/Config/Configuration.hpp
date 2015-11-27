#ifndef KRONOS_CONFIGURATION_HPP
#define KRONOS_CONFIGURATION_HPP

#include <Utils/Config/ConfigurationValue.hpp>

#include <QString>
#include <QMap>
#include <rapidjson/document.h>
#include <exception>

/**
 * Super class for all exceptions thrown while a requested configuration value
 * is being fetched.
 */
struct ConfigurationException : public std::exception {
	std::string reason;

	ConfigurationException(QString reason) : reason(reason.toStdString()) { }

	const char *what() const noexcept override {
		return reason.c_str();
	}
};

/**
 * Exception thrown if the configuration file could not be opened.
 */
struct ConfigFileOpenException : public ConfigurationException {
	ConfigFileOpenException(QString path, QString errorDescription)
			: ConfigurationException(
				QString("The configuration file at '%1' could not be opened: %2")
				.arg(path).arg(errorDescription)
			) { }
};

/**
 * Exception thrown if the JSON content of the configuration file could not
 * be parsed.
 */
struct JsonParseException : public ConfigurationException {
	JsonParseException(QString path, QString errorDescription)
			: ConfigurationException(
				QString("The configuration file at '%1' could not be parsed: %2")
				.arg(path).arg(errorDescription)
			) { }
};

/**
 * Exception thrown if the requested key does not exist in the configuration
 * file.
 */
struct InvalidKeyException : public ConfigurationException {
	InvalidKeyException(QString key)
			: ConfigurationException(
				QString("The requested key '%1' could not be found in the configuration file.")
				.arg(key)
			) { }
};

/**
 * Exception thrown if the requested key's value type does not match the specified value type.
 */
struct InvalidValueException : public ConfigurationException {
	InvalidValueException(QString key, QString expectedValue, QString actualValue)
			: ConfigurationException(
				QString("A value of type %1 was requested, but the value of the "
                "requested key '%2' was of type %3.")
				.arg(expectedValue).arg(key).arg(actualValue)
			) { }
};

/**
 * Exception thrown if a value in the JSON file is not supported.
 */
struct UnsupportedValueException : public ConfigurationException {
	UnsupportedValueException(QString key) : ConfigurationException(
				QString("A value with key %1 in the configuration file is not supported.")
				.arg(key)
			) { }
};

class Configuration {
public:
	/**
	* Get the singleton instance of this class.
	* @return The singleton instance of this class
	*/
	static Configuration& getInstance();

	/**
	* Check whether the loaded configuration file contains a given key.
	* @param key Key to be tested for
	* @return True if the key is present in the configuration file, false otherwise
	*/
	bool hasKey(QString key) const;

	/**
	* Get a string value from the configuration file.
	* @param key The key of the value to get, which is a path through the data
	* tree of the JSON file, separated by dots.
	*/
	QString getString(QString key) const;

	/**
	* Get a boolean value from the configuration file.
	* @param key The key of the value to get
	*/
	bool getBoolean(QString key) const;

	/**
	 * Get an integer value from the configuration file.
	 * @param key The key of the value to get
	 */
	int getInteger(QString key) const;

	/**
	* Get a double value from the configuration file.
	* @param key The key of the value to get
	*/
	double getDouble(QString key) const;

private:
	/*
	* Hide some things that should not be accessed given this class uses
	* the singleton pattern.
	*/
	Configuration();
	Configuration(Configuration const&) = delete;
	void operator=(Configuration const&) = delete;

	/**
	 * Get a ConfigurationValue out of the map using the key while checking for
	 * its existence.
	 * @param key The key to be searched for
	 * @param type Type of the configuration value that will be used to check
	 * whether the returned value is of the right type
	 * @return The ConfigurationValue stored with the specified key
	 */
	ConfigurationValue getConfigurationValue(QString key, int type) const;

	/**
	* The path where the configuration file resides.
	*/
	const QString CONFIGURATION_FILE_PATH;

	/**
	 * A QMap that will store all configuration values by mapping them to their key
	 */
	QMap<QString, ConfigurationValue> values;

	/**
	 * Load the configuration file and remember all values for later use.
	 */
	void loadConfigurationFile();

	/**
	 * Recursively iterate through a JSON object tree saving all its leaves
	 * which are primitive data entries such as strings or numbers in the `values`
	 * map along with dot-separated paths to each of them.
	 * @param jsonValue A reference to a rapidjson::Value that will be used as
	 * a starting point for the depth-first search through the tree
	 * @param path The dot-separated path of the current recursive call
	 */
	void indexValues(rapidjson::Value& jsonValue, QString path);
};

#endif
