#ifndef KRONOS_CONFIGUTIL_HPP
#define KRONOS_CONFIGUTIL_HPP

#include <QString>
#include <QFile>
#include <QIODevice>

#include "ImageLayerDescription.hpp"

class ConfigUtil {
public:
  /**
   * Load layer information from a file.
   * @param file Path of the file to be read
   * @return A list of layers as read from the file
   */
  static const QList<ImageLayerDescription> loadConfigFile(const QString &file);
};

#endif
