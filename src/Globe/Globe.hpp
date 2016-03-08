#ifndef STUPRO_GLOBE_HPP
#define STUPRO_GLOBE_HPP

#include <Globe/GlobeTile.hpp>
#include <Utils/Graphics/ResourcePool.hpp>
#include <Utils/Math/Vector2.hpp>
#include <Utils/Misc/SlotCallback.hpp>
#include <Utils/TileDownload/ImageDownloader.hpp>
#include <Utils/TileDownload/ImageTile.hpp>
#include <vtkOpenGLTexture.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <QEventLoop>
#include <QTimer>
#include <mutex>
#include <queue>
#include <array>
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
	Globe(vtkRenderer& renderer);

	/**
	 * Virtual destructor.
	 */
	virtual ~Globe();

	/**
	 * Returns the plane mapper with an appropriate level of detail for the terrain height range in
	 * a tile.
	 *
	 * @param heightDifference The difference between the minimum and maximum height in a tile.
	 * @return the mapper responsible for rendering the tiles at the specified level of detail.
	 */
	vtkSmartPointer<vtkPolyDataMapper> getPlaneMapper(float heightDifference) const;

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
	 * Prevents the zoom level from being changed until the corresponding unlock function is called.
	 */
	void lockZoomLevel();
	
	/**
	 * Allows the zoom level to change again after the unlock function is called.
	 */
	void unlockZoomLevel();
	
	/**
	 * @return whether the zoom level is currently locked.
	 */
	bool isZoomLevelLocked() const;

	/**
	 * Enum holding the available display modes for the globe.
	 */
	enum DisplayMode {
		DisplayGlobe = 0, DisplayMap = 1
	};

	/**
	 * Changes the globe's display mode. This changes the globe between a flat map view and a
	 * spherical globe view.
	 *
	 * @param displayMode The display mode to use for the globe
	 */
	void setDisplayMode(DisplayMode displayMode);

	/**
	 * @return the globe's current display mode.
	 */
	DisplayMode getDisplayMode() const;

	/**
	 * @return the current animation state of the globe's display mode between 0.0 (globe) and 1.0
	 *         (map).
	 */
	float getDisplayModeInterpolation() const;

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
	 * Returns a resource pool handle to a specific tile from the globe. The lon/lat pair is given
	 * in tile indices starting from 0 and is normalized/wrapped around the world before selecting
	 * the tile.
	 *
	 * @param lon The integer longitude to get the tile at
	 * @param lat The integer latitiude to get the tile at
	 *
	 * @return a handle to the globe tile at the specified longitude/latitude index
	 */
	ResourcePool<GlobeTile>::Handle getTileHandleAt(int lon, int lat) const;

	/**
	 * Assigns the globe tile handle at the specified tile coordinate.
	 *
	 * @param lon The integer longitude to set the tile at
	 * @param lat The integer latitiude to set the tile at
	 * @param handle The tile handle to assign
	 */
	void setTileHandleAt(int lon, int lat, ResourcePool<GlobeTile>::Handle handle);

	/**
	 * Checks if the specified tile is facing towards the camera and within the camera's view frustum.
	 */
	bool isTileInViewFrustum(int lon, int lat, vtkMatrix4x4* normalTransform,
	                         vtkMatrix4x4* compositeTransform) const;

	/**
	 * Changes the visibility of the specified tile.
	 */
	void setTileVisibility(int lon, int lat, bool visibility);

	/**
	 * Sets the visibility of a tile to true.
	 */
	void showTile(int lon, int lat);

	/**
	 * Sets the visibility of a tile to false.
	 */
	void hideTile(int lon, int lat);

	/**
	 * Resizes the tile handle list to the current zoom level.
	 */
	void createTileHandles();

	/**
	 * Hides all currently visible globe tiles and erases the handles.
	 */
	void eraseTileHandles();

	/**
	 * Loads all fetched globe tiles.
	 */
	void loadGlobeTiles();

	/**
	 * Loads a globe tile from an ImageTile.
	 */
	void loadGlobeTile(const ImageTile& tile);

	/**
	 * Updates the globe's display mode interpolation value for a smooth animation.
	 *
	 * @param instant If this is true, the display mode will be immediately adjusted to its target
	 *                value.
	 */
	void updateDisplayMode(bool instant);

	/**
	 * Checks which level of detail is required for globe tiles and reloads them with that LOD.
	 */
	void updateZoomLevel();

	/**
	 * Checks if the camera has changed since the last call to this function. If this is the case,
	 * checks which globe tiles are invisible and need to be culled.
	 */
	void updateTileVisibilityIfNeeded();

	/**
	 * Checks which globe tiles are invisible and need to be culled.
	 */
	void updateTileVisibility();

	/**
	 * Checks which globe tiles are invisible and need to be culled (implementation function).
	 */
	void updateTileVisibilityImpl(bool forceUpdate);

	/**
	 * Generates the LOD table for height difference -> resolution mapping.
	 */
	void generateLODTable();

	void onTileLoad(ImageTile tile);

	vtkRenderer& myRenderer;

	vtkSmartPointer<vtkOpenGLTexture> myLoadingTexture;

	ImageDownloader myDownloader;
	std::queue<ImageTile> myDownloadedTiles;
	std::mutex myDownloadedTilesMutex;

	std::unique_ptr<QTimer> myTimer;
	SlotCallback myTimerCallback;

	ResourcePool<GlobeTile> myTilePool;
	std::vector<ResourcePool<GlobeTile>::Handle> myTileHandles;

	/**
	 * Map from minimum terrain height difference to plane source resolution.
	 */
	struct LODSetting {
		LODSetting(float heightRange, unsigned int lod);

		float heightRange;
		unsigned int lod;
		vtkSmartPointer<vtkPlaneSource> planeSource;
		vtkSmartPointer<vtkPolyDataMapper> planeMapper;
	};

	std::array<double, 16> myCachedCameraMatrix;

	std::vector<LODSetting> myLODTable;

	unsigned int myZoomLevel;
	bool myIsZoomLevelLocked;

	DisplayMode myDisplayMode;
	float myDisplayModeInterpolation;
};

#endif
