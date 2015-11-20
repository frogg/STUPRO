#include <Utils/TileDownload/ImageLayerDescription.hpp>

#include <climits>

ImageLayerDescription::ImageLayerDescription(QString baseUrl, QString mimeType, int tileSize,
    QList<LayerStep> layerSteps)
    : baseUrl(baseUrl), mimeType(mimeType), tileSize(tileSize), layerSteps(layerSteps) { }

QString ImageLayerDescription::buildTileUrl(int zoomLevel, int tileX, int tileY) const {
    // find the best fitting layer step
    LayerStep step = { INT_MIN, "" };
    for (int i = 0; i < this->layerSteps.size(); i++) {
        if (this->layerSteps[i].minZoomLevel > step.minZoomLevel) {
            if (this->layerSteps[i].minZoomLevel <= zoomLevel) {
                step = this->layerSteps[i];
            }
        }
    }

    // build the URL for the API request
    return this->getBaseUrl()
        + "&layers=" + step.layers
        + "&format=" + this->getMimeType()
        + "&width=" + QString::number(this->getTileSize())
        + "&height=" + QString::number(this->getTileSize())
        + "&bbox=" + this->getBoundingBoxString(zoomLevel, tileX, tileY);
}

QString ImageLayerDescription::getBoundingBoxString(int zoomLevel, int tileX, int tileY) {
	double tileWidth = getTileWidthAtZoomLevel(zoomLevel);
	double tileHeight = getTileHeightAtZoomLevel(zoomLevel);

	double latMin = 90.0 - (tileHeight * (tileY + 1));
	double latMax = 90.0 - (tileHeight * tileY);
	double longMin = (tileWidth * tileX) - 180.0;
	double longMax = (tileWidth * (tileX + 1)) - 180.0;

	QString boundingBoxString("%1,%3,%2,%4");
	boundingBoxString = boundingBoxString.arg(QString::number(longMin), QString::number(longMax));
	boundingBoxString = boundingBoxString.arg(QString::number(latMin), QString::number(latMax));

	return boundingBoxString;
}

void ImageLayerDescription::getTilePositionFromCoordinates(double latitude, double longitude, int zoomLevel,
        int &tileX, int &tileY) {
    double tileWidth = getTileWidthAtZoomLevel(zoomLevel);
    double tileHeight = getTileHeightAtZoomLevel(zoomLevel);

    tileX = (int)((180.0 + longitude) / tileWidth);
    tileY = (int)((90.0 - latitude) / tileHeight);
}


void ImageLayerDescription::validateTileLocation(int zoomLevel, int tileX, int tileY) {
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

QString ImageLayerDescription::getBaseUrl() const {
    return this->baseUrl;
}

QString ImageLayerDescription::getMimeType() const {
    return this->mimeType;
}

int ImageLayerDescription::getTileSize() const {
    return this->tileSize;
}

QList<LayerStep> ImageLayerDescription::getLayerSteps() const {
    return this->layerSteps;
}

int ImageLayerDescription::getHorizontalTilesAtZoomLevel(int zoomLevel) {
    return WIDTH_AT_MIN_ZOOM << (zoomLevel - MIN_ZOOM_LEVEL);
}

int ImageLayerDescription::getVerticalTilesAtZoomLevel(int zoomLevel) {
    return HEIGHT_AT_MIN_ZOOM << (zoomLevel - MIN_ZOOM_LEVEL);
}

double ImageLayerDescription::getTileWidthAtZoomLevel(int zoomLevel) {
    return 360.0 / (double)getHorizontalTilesAtZoomLevel(zoomLevel);
}

double ImageLayerDescription::getTileHeightAtZoomLevel(int zoomLevel) {
    return 180.0 / (double)getVerticalTilesAtZoomLevel(zoomLevel);
}
