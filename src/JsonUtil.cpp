#include "../include/JsonUtil.hpp"

#include <QString>
#include <QFile>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

const QJsonObject JsonUtil::loadJsonFile(const QString &file)
{
	QString fileContent;
  QFile jsonFile;
  jsonFile.setFileName(file);
  jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
  fileContent = jsonFile.readAll();
  jsonFile.close();
  QJsonDocument document = QJsonDocument::fromJson(fileContent.toUtf8());
  return document.object();
}
