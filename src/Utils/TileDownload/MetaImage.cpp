#include <Utils/TileDownload/MetaImage.hpp>

MetaImage::MetaImage(const QImage& image, short minimumHeight, short maximumHeight)
	: image(image), minimumHeight(minimumHeight), maximumHeight(maximumHeight),
	  metaDataAttached(true) { }

MetaImage::MetaImage(const QImage& image)
	: image(image), minimumHeight(0), maximumHeight(0), metaDataAttached(false) { }

MetaImage::MetaImage() { }

const QImage& MetaImage::getImage() {
	return this->image;
}

void MetaImage::setImage(const QImage& image) {
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
