#ifndef KRONOS_JSONUTIL_HPP
#define KRONOS_JSONUTIL_HPP

#include <QString>
#include <QFile>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

class JsonUtil {
public:
  /**
   * Load a JSON object from a file.
   * @param file Path of the file to be read
   * @return A JSON object as read from the file
   */
  static const QJsonObject loadJsonFile(const QString &file);
};

#endif
