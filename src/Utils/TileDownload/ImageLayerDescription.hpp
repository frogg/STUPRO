#ifndef KRONOS_UTILS_TILEDOWNLOAD_IMAGELAYERDESCRIPTION_HPP
#define KRONOS_UTILS_TILEDOWNLOAD_IMAGELAYERDESCRIPTION_HPP

#include <Utils/TileDownload/LayerStep.hpp>

#include <QString>
#include <QList>


/**
 * Exception thrown when a tile location parameter is out of range.
 */
struct InvalidTileLocationException : public std::exception {
    std::string reason;

    InvalidTileLocationException(QString message) : reason(message.toStdString()) { }

    const char * what() const noexcept override {
        return reason.c_str();
    }
};

/**
 * Exception thrown when a tile's zoom level is out of range.
 */
struct InvalidTileZoomException : public InvalidTileLocationException {
    InvalidTileZoomException(int given, int min, int max)
        : InvalidTileLocationException(
            QString("The given zoomLevel is invalid. Expected value between (including) %2 and %3. Was given %1.")
            .arg(given, min, max)
        ) { }
};

/**
 * Exception thrown when a tile's x or y position is out of range.
 */
struct InvalidTilePositionException : public InvalidTileLocationException {
    InvalidTilePositionException(int zoomLevel, int xMin, int xMax, int xActual, int yMin, int yMax, int yActual)
        : InvalidTileLocationException(
            QString("The given tile position is invalid. Valid tile positions for zoom level %1 must be within x%2 to x%3 and y%5 to y%6. Was given x%4 y%7.")
            .arg(
                QString::number(zoomLevel),
                QString::number(xMin), QString::number(xMax), QString::number(xActual),
                QString::number(yMin), QString::number(yMax), QString::number(yActual)
            )
        ) { }
};

/**
 * This class holds information containing a whole image layer and how it
 * should be accessed using the API, all specified in the configuration JSON
 * file.
 * On top of that this class includes some static helper methods for API
 * access.
 */
class ImageLayerDescription {
public:
    static const int MIN_ZOOM_LEVEL = 0;
    static const int MAX_ZOOM_LEVEL = 15;
    static const int WIDTH_AT_MIN_ZOOM = 2;
    static const int HEIGHT_AT_MIN_ZOOM = 1;

    /**
     * Creates a new ImageLayerDescription holding the given information.
     *
     * @param baseUrl    the baseUrl for the API calls
     * @param mimeType   the mime-type to request from the API
     * @param tileSize   the tile-size to request from the API
     * @param layerSteps the available layer steps for the image layer
     */
    ImageLayerDescription(QString baseUrl, QString mimeType, int tileSize, QList<LayerStep> layerSteps);
    ImageLayerDescription() { }
    ~ImageLayerDescription() { }

    /**
     * Builds the API request URL for the tile at the given location
     *
     * @param zoomLevel how deep to dive into the quad-tree
     * @param tileX     horizontal position of the requested tile (westernmost tile = 0)
     * @param tileY     vertical position of the requested tile (northernmost tile = 0)
     *
     * @returns the API request URL for the tile at the given location
     */
    QString buildTileUrl(int zoomLevel, int tileX, int tileY) const;

    /**
     * Calculates the bounding box in lat-long from the given tile location.
     * Assumes that the given tile location is valid.
     *
     * @param zoomLevel how deep to dive into the quad-tree
     * @param tileX     horizontal position of the requested tile (westernmost tile = 0)
     * @param tileY     vertical position of the requested tile (northernmost tile = 0)
     *
     * @returns a string representation of the lat-long bounding box
     */
    static QString getBoundingBoxString(int zoomLevel, int tileX, int tileY);

    /**
     * Calculates the x and y position of the tile at the given coordinates with the given zoom
     * level.
     *
     * @param latitude  the latitude part of the coordinate
     * @param longitude the longitude part of the coordinate
     * @param zoomLevel the desired zoomLevel
     * @param tileX     will contain the x position of the tile
     * @param tileY     will contain the y position of the tile
     */
    static void getTilePositionFromCoordinates(double latitude, double longitude, int zoomLevel, int &tileX, int &tileY);

    /**
     * Checks if the given tile location is valid for the underlying quad-tree.
     *
     * @param zoomLevel how deep to dive into the quad-tree
     * @param tileX     horizontal position of the requested tile (westernmost tile = 0)
     * @param tileY     vertical position of the requested tile (northernmost tile = 0)
     *
     * @throws InvalidTileZoomException if the zoom level is out of bounds
     * @throws InvalidTilePosition      if the x or y position of the tile is out of bounds for the
     *                                  given zoom level
     */
    static void validateTileLocation(int zoomLevel, int tileX, int tileY);

    /**
     * @returns The baseUrl for the API calls
     */
    QString getBaseUrl() const;

    /**
     * @returns The mime-type for the API calls
     */
    QString getMimeType() const;

    /**
     * @returns The tile-size for the API calls
     */
    int getTileSize() const;

    /**
     * @returns The layer steps for the API calls
     */
    QList<LayerStep> getLayerSteps() const;

private:
    QString baseUrl;
    QString mimeType;
    int tileSize;
    QList<LayerStep> layerSteps;

    /**
     * Calculates the number of tiles on the horizontal axis at the given zoomLevel.
     */
    static int getHorizontalTilesAtZoomLevel(int zoomLevel);

    /**
     * Calculates the number of tiles on the vertical axis at the given zoomLevel.
     */
    static int getVerticalTilesAtZoomLevel(int zoomLevel);

    /**
     * Calculates the width of tiles at the given zoomLevel.
     */
    static double getTileWidthAtZoomLevel(int zoomLevel);

    /**
     * Calculates the height of tiles at the given zoomLevel.
     */
    static double getTileHeightAtZoomLevel(int zoomLevel);
};

#endif
