#ifndef KRONOS_CONFIGUTIL_HPP
#define KRONOS_CONFIGUTIL_HPP

#include <qmap.h>
#include <qstring.h>
#include <Utils/TileDownload/ImageLayerDescription.hpp>
#include <exception>
#include <string>

struct FileOpenException : public std::exception
{
	QString message;

	FileOpenException(QString message) : message(message) { }

  const char * what () const throw ()
  {
    return message.toStdString().c_str();
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
};

#endif
