#include "../include/ImageTileFetcher.hpp"

#include "../include/ImageCache.hpp"
#include "../include/ImageDownloadWorker.hpp"

ImageTileFetcher::ImageTileFetcher(QMap<QString, ImageLayerDescription> availableLayers,
								   QList<QString> requestedLayers, int zoomLevel, int tileX, int tileY,
								   ImageDownloader::TileFetchedCb tileFetchedCb) {
	validateTileLocation(zoomLevel, tileX, tileY);
	validateLayersAvailable(availableLayers.keys(), requestedLayers);

	this->availableLayers = availableLayers;
	this->requestedLayers = requestedLayers;
	this->zoomLevel = zoomLevel;
	this->tileX = tileX;
	this->tileY = tileY;
	this->tileFetchedCb = tileFetchedCb;
}

ImageTileFetcher::~ImageTileFetcher() { }

void ImageTileFetcher::run() {
	// check every image on the requested tile for a cache hit, load it from the cache if possible,
	// otherwise kick of an ImageDownloadWorker
	ImageCache &cache = ImageCache::getInstance();
	QMap<QString, MetaImage> images2;
	std::map<QString, MetaImage> images;
	std::vector<ImageDownloadWorker *> workers;

	for (auto const &layer : this->requestedLayers) {
		if (cache.isImageCached(layer, this->zoomLevel, this->tileX, this->tileY)) {
			// load the image from the cache
			MetaImage image = cache.getCachedImage(layer, this->zoomLevel, this->tileX, this->tileY);
			// images.insert(layer, image);
			images.insert(std::pair<QString, MetaImage>(layer, image));
		} else {
			// kick off an ImageDownloadWorker
			QUrl imageUrl = this->buildTileDownloadUrl(layer);
			int tileSize = this->availableLayers[layer].tileSize;
			// not-so-nice pointers, but didn't get it to work otherwise :/
			ImageDownloadWorker *worker = new ImageDownloadWorker(layer, imageUrl, tileSize, tileSize);
			workers.push_back(worker);
		}
	}

	for (auto *worker : workers) {
		MetaImage image = worker->getFuture().get();
		images.insert(std::pair<QString, MetaImage>(worker->getLayerName(), image));
		cache.cacheImage(image, worker->getLayerName(), this->zoomLevel, this->tileX, this->tileY);
		delete worker;
	}

	ImageTile tile(QMap<QString, MetaImage>(images), this->zoomLevel, this->tileX, this->tileY);
	this->tileFetchedCb(tile);
}

QString ImageTileFetcher::calculateBoundingBox(int zoomLevel, int tileX, int tileY) {
	int horizontalTilesAtZoomLevel = WIDTH_AT_MIN_ZOOM << (zoomLevel - MIN_ZOOM_LEVEL);
	int verticalTilesAtZoomLevel = HEIGHT_AT_MIN_ZOOM << (zoomLevel - MIN_ZOOM_LEVEL);

	float tileWidthAtZoomLevel = 360.0 / (float)horizontalTilesAtZoomLevel;
	float tileHeightAtZoomLevel = 180.0 / (float)verticalTilesAtZoomLevel;

	float latMin = 90.0 - (tileHeightAtZoomLevel * (tileY + 1));
	float latMax = 90.0 - (tileHeightAtZoomLevel * tileY);
	float longMin = (tileWidthAtZoomLevel * tileX) - 180.0;
	float longMax = (tileWidthAtZoomLevel * (tileX + 1)) - 180.0;

	QString ret("%1,%3,%2,%4");
	ret = ret.arg(QString::number(longMin), QString::number(longMax));
	ret = ret.arg(QString::number(latMin), QString::number(latMax));

	return ret;
}

QUrl ImageTileFetcher::buildTileDownloadUrl(const QString layer) const {
	ImageLayerDescription layerDescription = this->availableLayers[layer];
	QString urlString = layerDescription.baseUrl;

	// add format parameters
	urlString += "&format=" + QString(layerDescription.format);

	// add tile size parameters
	urlString += "&width=" + QString::number(layerDescription.tileSize);
	urlString += "&height=" + QString::number(layerDescription.tileSize);

	// add bounding box parameters
	urlString += "&bbox=" + calculateBoundingBox(this->zoomLevel, this->tileX, this->tileY);

	return QUrl(urlString);
}

void ImageTileFetcher::validateTileLocation(int zoomLevel, int tileX, int tileY) {
	// check zoom level range
	if (zoomLevel < MIN_ZOOM_LEVEL || zoomLevel > MAX_ZOOM_LEVEL) {
		throw InvalidTileZoomException(zoomLevel, MIN_ZOOM_LEVEL, MAX_ZOOM_LEVEL);
	}

	// check tile position range
	int maxX = (WIDTH_AT_MIN_ZOOM << (zoomLevel - MIN_ZOOM_LEVEL)) - 1;
	int maxY = (HEIGHT_AT_MIN_ZOOM << (zoomLevel - MIN_ZOOM_LEVEL)) - 1;
	if (tileX < 0 || tileX > maxX || tileY < 0 || tileY > maxY) {
		throw InvalidTilePositionException(zoomLevel, 0, maxX, tileX, 0, maxY, tileY);
	}
}

void ImageTileFetcher::validateLayersAvailable(const QList<QString> availableLayers,
		const QList<QString> requestedLayers) {
	for (QString layer : requestedLayers) {
		if (!availableLayers.contains(layer)) {
			throw InvalidLayerException(layer, availableLayers);
		}
	}
}
