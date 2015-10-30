#include "../include/ImageCache.hpp"

#include <QImage>
#include <QFile>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QList>
#include <QJsonObject>

ImageCache::ImageCache(QJsonObject* configuration) {
  // TODO: Read configuration
}

ImageCache::ImageCache() : ImageCache() { }

ImageCache::~ImageCache() {
  // TODO: Delete stuff
}

void ImageCache::cacheImage(QImage* image, QString* layer, int zoomLevel, int tileX, int tileY) {

}
