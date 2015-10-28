#include "../include/ImageTile.hpp"

#include <QImage>
#include <QString>
#include <QList>
#include <QMap>

ImageTile::ImageTile(const QMap<QString, QImage>& layers, int zoomLevel, int tileX, int tileY) {

}

ImageTile::~ImageTile() {
	delete this->layers;
  delete this->zoomLevel;
  delete this->tileX;
  delete this->tileY;
}

QMap<QString, QImage>& ImageTile::getLayers() {
  return this->layers;
}

void ImageTile::setLayers(const QMap<QString, QImage>& layers) {
  this->layers = layers;
}

int ImageTile::getZoomLevel() {
  return this->zoomLevel;
}

void ImageTile::setZoomLevel(int zoomLevel) {
  this->zoomLevel = zoomLevel;
}

int ImageTile::getTileX() {
  return this->tileX;
}

void ImageTile::setTileX(int tileX) {
  this->tileX = tileX;
}

int ImageTile::getTileY() {
  return this->tileY;
}

void ImageTile::setTileY(int tileY) {
  this->tileY = tileY;
}
