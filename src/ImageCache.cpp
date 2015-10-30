#include "../include/ImageCache.hpp"
#include "../include/ImageLayerDescription.hpp"

#include <QImage>
#include <QFile>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QList>

ImageCache::ImageCache(QList<ImageLayerDescription> imageLayers) {
  // TODO: Read configuration
}

ImageCache::ImageCache() : ImageCache() { }

ImageCache::~ImageCache() {
  // TODO: Delete stuff
}

void ImageCache::cacheImage(QImage* image, QString* layer, int zoomLevel, int tileX, int tileY) {

}
