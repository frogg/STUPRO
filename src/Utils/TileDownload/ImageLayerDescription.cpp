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
        + "&bbox=" + this->calculateBoundingBox(zoomLevel, tileX, tileY);
}

QString ImageLayerDescription::calculateBoundingBox(int zoomLevel, int tileX, int tileY) {
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
