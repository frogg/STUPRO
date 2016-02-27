#include <Utils/TileDownload/ImageDownloader.hpp>

#include <Utils/Misc/KronosLogger.hpp>
#include <Utils/Misc/MakeUnique.hpp>
#include <Utils/Misc/ServerUtils.hpp>
#include <Utils/TileDownload/ClientTileRequestWorker.hpp>
#include <Utils/TileDownload/ServerTileRequestWorker.hpp>

#include <thread>

ImageDownloader::ImageDownloader(OnTileFetched onTileFetched)
	: ImageDownloader(onTileFetched, ImageDownloader::defaultErrorHandler) { }

ImageDownloader::ImageDownloader(OnTileFetched onTileFetched, QSet<QString> requestedLayers)
	: ImageDownloader(onTileFetched, ImageDownloader::defaultErrorHandler, requestedLayers) { }

ImageDownloader::ImageDownloader(OnTileFetched onTileFetched, OnTileFetchFailed onTileFetchFailed)
	: ImageDownloader(onTileFetched, onTileFetchFailed, ImageDownloader::getAllAvailableLayers()) { }

ImageDownloader::ImageDownloader(OnTileFetched onTileFetched, OnTileFetchFailed onTileFetchFailed,
                                 QSet<QString> requestedLayers) {
	if (ServerUtils::isClient()) {
		this->requestWorker = std::unique_ptr<TileRequestWorker>(new ClientTileRequestWorker(
		                          requestedLayers, onTileFetched, onTileFetchFailed));
	} else {
		this->requestWorker = std::unique_ptr<TileRequestWorker>(new ServerTileRequestWorker(
		                          requestedLayers, onTileFetched, onTileFetchFailed));
	}
}

void ImageDownloader::requestTile(int zoomLevel, int tileX, int tileY) {
	this->requestWorker->requestTile(zoomLevel, tileX, tileY);
}

void ImageDownloader::abortAllRequests() {
	this->requestWorker->requestAbort();
}

const QSet<QString> ImageDownloader::getRequestedLayers() const {
	return this->requestWorker->getRequestedLayers();
}

void ImageDownloader::defaultErrorHandler(std::exception const& e) {
	KRONOS_LOG_WARN("%s", e.what());
}

QSet<QString> ImageDownloader::getAllAvailableLayers() {
	return QSet<QString>::fromList(ConfigUtil::loadConfigFile("./res/layers.json").keys());
}
