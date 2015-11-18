#ifndef KRONOS_CONFIGUTIL_HPP
#define KRONOS_CONFIGUTIL_HPP

#include <qmap.h>
#include <qstring.h>
#include <Utils/TileDownload/ImageLayerDescription.hpp>
#include <exception>
#include <string>

/**
 * Exception that will be thrown whenever a configuration file could not be
 * opened.
 */
struct FileOpenException : public std::exception {
	std::string reason;

	FileOpenException(QString message) : reason(message.toStdString()) { }

	const char *what() const noexcept override {
		return reason.c_str();
	}
};

struct JsonParseException : public std::exception {
	std::string reason;

	JsonParseException(QString message) : reason(message.toStdString()) { }

	const char *what() const noexcept override {
		return reason.c_str();
	}
};

class ConfigUtil {
public:
  /**
   * Load layer information from a file.
   * @param file Path of the file to be read
   * @return A map of layer identifiers and layers as read from the file
	 * @throws FileOpenException If the config could not be opened
   */
  static const QMap<QString, ImageLayerDescription> loadConfigFile(const QString &file);

private:
	/**
	 * Error message used if the requested configuration file does not exist.
	 */
	static const QString FILE_DOES_NOT_EXIST_MESSAGE;

	/**
	 * Error message used if the requested configuration file could not be opened.
	 */
	static const QString FILE_COULD_NOT_BE_OPENED_MESSAGE;

	/**
	 * Error message used if the JSON data the requested configuration file contains
	 * could not be parsed.
	 */
	static const QString FILE_COULD_NOT_BE_PARSED_MESSAGE;
};

#endif
