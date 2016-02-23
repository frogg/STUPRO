#include <Utils/TileDownload/MetaImage.hpp>

MetaImage::MetaImage(const QImage& image, short minimumHeight, short maximumHeight)
	: image(image), minimumHeight(minimumHeight), maximumHeight(maximumHeight),
	  metaDataAttached(true) { }

MetaImage::MetaImage(const QImage& image)
	: image(image), minimumHeight(0), maximumHeight(0), metaDataAttached(false) { }

MetaImage::MetaImage() { }

const QImage& MetaImage::getImage() const {
	return this->image;
}

void MetaImage::setImage(const QImage& image) {
	this->image = image;
}

short MetaImage::getMinimumHeight() const {
	return this->minimumHeight;
}

short MetaImage::getMaximumHeight() const {
	return this->maximumHeight;
}

void MetaImage::setMetaData(short minimumHeight, short maximumHeight) {
	this->metaDataAttached = true;
	this->minimumHeight = minimumHeight;
	this->maximumHeight = maximumHeight;
}

bool MetaImage::hasMetaData() const {
	return this->metaDataAttached;
}
