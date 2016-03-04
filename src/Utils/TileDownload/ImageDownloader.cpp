#include <Utils/TileDownload/ImageDownloader.hpp>

#include <Utils/Misc/KronosLogger.hpp>
#include <Utils/Misc/MakeUnique.hpp>
#include <Utils/Misc/ServerUtils.hpp>
#include <Utils/TileDownload/ClientTileRequestWorker.hpp>
#include <Utils/TileDownload/ServerTileRequestWorker.hpp>

#include <thread>

ImageDownloader::ImageDownloader(OnTileFetched onTileFetched, QString configFile)
	: ImageDownloader(onTileFetched, ImageDownloader::defaultErrorHandler, configFile) { }

ImageDownloader::ImageDownloader(OnTileFetched onTileFetched, QSet<QString> requestedLayers,
                                 QString configFile)
	: ImageDownloader(onTileFetched, ImageDownloader::defaultErrorHandler, requestedLayers,
	                  configFile) { }

ImageDownloader::ImageDownloader(OnTileFetched onTileFetched, OnTileFetchFailed onTileFetchFailed,
                                 QString configFile)
	: ImageDownloader(onTileFetched, onTileFetchFailed, ImageDownloader::getAllAvailableLayers(),
	                  configFile) { }

ImageDownloader::ImageDownloader(OnTileFetched onTileFetched, OnTileFetchFailed onTileFetchFailed,
                                 QSet<QString> requestedLayers, QString configFile) {
	if (ServerUtils::isClient()) {
		this->requestWorker = makeUnique<ClientTileRequestWorker>(requestedLayers, onTileFetched,
		                      onTileFetchFailed, configFile);
	} else {
		this->requestWorker = makeUniqur<ServerTileRequestWorker>(requestedLayers, onTileFetched,
		                      onTileFetchFailed, configFile));
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
