#include "../include/ImageDownloader.hpp"

ImageDownloader::ImageDownloader(tileFetchedCbType imageFetchedCb)
	: ImageDownloader(imageFetchedCb, NULL) { }

ImageDownloader::ImageDownloader(tileFetchedCbType imageFetchedCb, QJsonObject *configuration)
{
	this->imageFetchedCb = imageFetchedCb;
}

ImageDownloader::~ImageDownloader()
{

}

QImage *ImageDownloader::getImage(QString *layer, int zoomLevel, int tileX, int tileY)
{

}

QList<QString> ImageDownloader::getAvailableLayers()
{

}

QImage *ImageDownloader::downloadImage(QUrl *imageUrl)
{

}
