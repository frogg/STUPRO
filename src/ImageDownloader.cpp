#include "../include/ImageDownloader.hpp"

QNetworkAccessManager ImageDownloader::networkManager;

ImageDownloader::ImageDownloader(tileFetchedCbType imageFetchedCb) {
	this->imageFetchedCb = imageFetchedCb;
}

ImageDownloader::ImageDownloader(tileFetchedCbType imageFetchedCb, QJsonObject *configuration) {
	this->imageFetchedCb = imageFetchedCb;
	this->config = configuration;
}

ImageDownloader::~ImageDownloader() {

}

QImage *ImageDownloader::getImage(QString *layer, int zoomLevel, int tileX, int tileY) {
	QMap<QString, QImage> layers;
	ImageTile tile(layers, zoomLevel, tileX, tileY);
	this->imageFetchedCb(tile);
}

QList<QString> ImageDownloader::getAvailableLayers() {
	QList<QString> availableLayers;
	availableLayers.append("satellite-imagery");
	availableLayers.append("heightmap");
	return availableLayers;
}

QImage *ImageDownloader::downloadImage(QUrl *imageUrl) {

}
