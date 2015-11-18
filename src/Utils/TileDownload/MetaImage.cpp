#include <Utils/TileDownload/MetaImage.hpp>

MetaImage::MetaImage(const QImage &image, short minimumHeight, short maximumHeight) {
	this->setImage(image);
	this->setMetaData(minimumHeight, maximumHeight);
}

MetaImage::MetaImage(const QImage &image) {
	this->setImage(image);
	this->minimumHeight = 0;
	this->maximumHeight = 0;
}

MetaImage::MetaImage() { }

MetaImage::~MetaImage() { }

const QImage &MetaImage::getImage() {
	return this->image;
}

void MetaImage::setImage(const QImage &image) {
	this->image = image;
}

short MetaImage::getMinimumHeight() {
	return this->minimumHeight;
}

short MetaImage::getMaximumHeight() {
	return this->maximumHeight;
}

void MetaImage::setMetaData(short minimumHeight, short maximumHeight) {
	this->metaDataAttached = true;
	this->minimumHeight = minimumHeight;
	this->maximumHeight = maximumHeight;
}

bool MetaImage::hasMetaData() {
	return this->metaDataAttached;
}
