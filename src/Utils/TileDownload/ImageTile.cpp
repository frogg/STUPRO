#include "ImageTile.hpp"

ImageTile::ImageTile(const QMap<QString, MetaImage> layers, int zoomLevel, int tileX,
					 int tileY) {
	this->setLayers(layers);
	this->setZoomLevel(zoomLevel);
	this->setTileX(tileX);
	this->setTileY(tileY);
}

ImageTile::~ImageTile() {
	this->layers.clear();
}

QMap<QString, MetaImage> &ImageTile::getLayers() {
	return this->layers;
}

void ImageTile::setLayers(const QMap<QString, MetaImage> layers) {
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
