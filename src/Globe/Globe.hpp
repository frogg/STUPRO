#ifndef STUPRO_GLOBE_HPP
#define STUPRO_GLOBE_HPP

#include <Globe/GlobeConfig.hpp>
#include <Utils/Math/Vector2.hpp>
#include <Utils/TileDownload/ImageDownloader.hpp>
#include <vtkOpenGLTexture.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <atomic>
#include <memory>
#include <vector>

class GlobeTile;

/**
 * A class for rendering a dynamically textured globe to a vtkRenderer.
 */
class Globe {
public:

	/**
	 * Creates the globe using the specified renderer and configuration.
	 */
	Globe(vtkRenderer& renderer, GlobeConfig globeConfig);

	/**
	 * Virtual destructor.
	 */
	virtual ~Globe();

	/**
	 * Assigns the globe configuration data for this globe.
	 *
	 * @param globeConfig The configuration data to assign to this globe
	 */
	void setGlobeConfig(GlobeConfig globeConfig);

	/**
	 * @return the globe configuration data for this globe.
	 */
	const GlobeConfig& getGlobeConfig() const;

	/**
	 * Changes the globe's vertex/heightmap resolution per tile.
	 *
	 * @param resolution The globe's resolution in grid units.
	 */
	void setResolution(Vector2u resolution);

	/**
	 * @return the globe's vertex/heightmap resolution per tile.
	 */
	Vector2u getResolution() const;

	/**
	 * @return the plane mapper responsible for rendering the tiles.
	 */
	vtkSmartPointer<vtkPolyDataMapper> getPlaneMapper() const;

	/**
	 * @return the render window associated with this globe
	 */
	vtkRenderWindow& getRenderWindow() const;

	/**
	 * @return the renderer associated with this globe
	 */
	vtkRenderer& getRenderer() const;

	/**
	 * Changes the zoom level of the globe tiles, determining the tile count and size.
	 *
	 * zoomLevel 0:  2x1 tiles
	 * zoomLevel 1:  4x2 tiles
	 * zoomLevel 2:  8x4 tiles
	 * zoomLevel 3: 16x8 tiles
	 * ...
	 *
	 * @param zoomLevel the globe's vertical tile cont in powers of two
	 */
	void setZoomLevel(unsigned int zoomLevel);

	/**
	 * @return the globe's vertical tile count in powers of two
	 */
	unsigned int getZoomLevel() const;

	/**
	 * Returns a specific tile from the globe. The lon/lat pair is given in tile indices starting
	 * from 0 and is normalized/wrapped around the world before selecting the tile.
	 *
	 * @param lon The integer longitude to get the tile at
	 * @param lat The integer latitiude to get the tile at
	 *
	 * @return the globe tile at the specified longitude/latitude index
	 */
	GlobeTile& getTileAt(int lon, int lat) const;

	/**
	 * Changes the display mode interpolation between globe view and map view. A value of 0.0 means
	 * globe, a value of 1.0 means map, and any values in-between result in a smooth animation
	 * between the two display modes.
	 *
	 * @param displayMode The interpolation between globe and map
	 */
	void setDisplayModeInterpolation(float displayMode);

	/**
	 * @return the current interpolation between globe and map.
	 */
	float getDisplayModeInterpolation() const;

	/**
	 * Returns true if a texture was loaded and a renderer re-paint is needed and resets the flag.
	 */
	bool checkIfRepaintIsNeeded();

	/**
	 * Checks which globe tiles are invisible and need to be culled.
	 */
	void onCameraChanged();

private:
	/**
	 * Returns the index of the specified tile within the tile array.
	 */
	unsigned int getTileIndex(int lon, int lat) const;

	/**
	 * Creates all tiles for the current zoom level of the globe.
	 */
	void createTiles();

	/**
	 * Checks which level of detail is required for globe tiles and reloads them with that LOD.
	 */
	void updateZoomLevel();

	/**
	 * Checks which globe tiles are invisible and need to be culled.
	 */
	void updateTileVisibility();

	void onTileLoad(ImageTile tile);

	vtkRenderer& myRenderer;

	GlobeConfig myGlobeConfig;

	vtkSmartPointer<vtkPlaneSource> myPlaneSource;
	vtkSmartPointer<vtkPolyDataMapper> myPlaneMapper;
	
	vtkSmartPointer<vtkOpenGLTexture> myLoadingTexture;

	ImageDownloader myDownloader;

	std::vector<std::unique_ptr<GlobeTile> > myTiles;

	unsigned int myZoomLevel;

	float myDisplayModeInterpolation;

	std::atomic_flag myIsClean;
};

#endif
