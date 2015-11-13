#ifndef KRONOS_UTILS_TILEDOWNLOAD_IMAGELAYERDESCRIPTION_HPP
#define KRONOS_UTILS_TILEDOWNLOAD_IMAGELAYERDESCRIPTION_HPP

#include <Utils/TileDownload/LayerStep.hpp>

#include <QString>
#include <QList>

class ImageLayerDescription {
public:
    /**
	 * Creates a new ImageLayerDescription holding the given information.
	 *
	 * @param baseUrl    the baseUrl for the API calls
	 * @param mimeType   the mime-type to request from the API
	 * @param tileSize   the tile-size to request from the API
	 * @param layerSteps the available layer steps for the image layer
	 */
    ImageLayerDescription(QString baseUrl, QString mimeType, int tileSize, QList<LayerStep> layerSteps);
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
    QString buildTileUrl(int zoomLevel, int tileX, int tileY);

    /**
     * @returns The baseUrl for the API calls
     */
    QString getBaseUrl();

    /**
     * @returns The mime-type for the API calls
     */
    QString getMimeType();

    /**
     * @returns The tile-size for the API calls
     */
    int getTileSize();

    /**
     * @returns The layer steps for the API calls
     */
    QList<LayerStep> getLayerSteps();

private:
    QString baseUrl;
    QString mimeType;
    int tileSize;
    QList<LayerStep> layerSteps;
};

#endif
