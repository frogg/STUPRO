#include <Utils/TileDownload/ImageTile.hpp>

ImageTile::ImageTile(const QMap<QString, MetaImage> layers, int zoomLevel, int tileX,
                     int tileY) {
	this->setLayers(layers);
	this->setZoomLevel(zoomLevel);
	this->setTileX(tileX);
	this->setTileY(tileY);
}

ImageTile::ImageTile(int zoomLevel, int tileX, int tileY)
        : zoomLevel(zoomLevel), tileX(tileX), tileY(tileY) { }

ImageTile::ImageTile() : ImageTile(QMap<QString, MetaImage>(), -1, -1, -1) { }

ImageTile::~ImageTile() {
	this->layers.clear();
}

const QMap<QString, MetaImage>& ImageTile::getLayers() const {
	return this->layers;
}

QMap<QString, MetaImage>& ImageTile::getLayers() {
    return this->layers;
}

void ImageTile::setLayers(const QMap<QString, MetaImage> layers) {
	this->layers = layers;
}

int ImageTile::getZoomLevel() const {
	return this->zoomLevel;
}

void ImageTile::setZoomLevel(int zoomLevel) {
	this->zoomLevel = zoomLevel;
}

int ImageTile::getTileX() const {
	return this->tileX;
}

void ImageTile::setTileX(int tileX) {
	this->tileX = tileX;
}

int ImageTile::getTileY() const {
	return this->tileY;
}

void ImageTile::setTileY(int tileY) {
	this->tileY = tileY;
}
