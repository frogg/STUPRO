#include "../include/ImageCache.hpp"
#include "../include/ConfigUtil.hpp"
#include "../include/ImageLayerDescription.hpp"

#include <QImage>
#include <QFile>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QMap>

ImageCache::ImageCache(QMap<QString, ImageLayerDescription> imageLayers) {
  // TODO: Read configuration
}

ImageCache::ImageCache() : ImageCache(ConfigUtil::loadConfigFile("./res/layers.cfg")) { }

ImageCache::~ImageCache() {
  // TODO: Delete stuff
}

void ImageCache::cacheImage(QImage* image, QString* layer, int zoomLevel, int tileX, int tileY) {

}
