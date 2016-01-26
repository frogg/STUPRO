#ifndef STUPRO_GLOBE_HPP
#define STUPRO_GLOBE_HPP

#include <Utils/Math/Vector2.hpp>
#include <Utils/Math/Vector3.hpp>
#include <Utils/TileDownload/ImageDownloader.hpp>
#include <Utils/Math/IncludeEigen.hpp>
#include <Globe/Coordinate.hpp>
#include <vtkOBBTree.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <atomic>
#include <memory>
#include <vector>

//TODO Configuration file
#define GLOBE_RADIUS 0.5
#define PLANE_WIDTH 4
#define PLANE_HEIGHT 2
#define PLANE_SIZE 1

class GlobeTile;

/**
 * A class for rendering a dynamically textured globe to a vtkRenderer.
 */
class Globe {
public:

	/**
	 * Creates the globe using the specified renderer.
	 */
	Globe(vtkRenderer& renderer);

	/**
	 * Virtual destructor.
	 */
	virtual ~Globe();

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
	void updateGlobeTileVisibility();

	/**
	 * Returns the Coordinates (lat, long) from multipe worldPoints of the Globe.
	 *
	 * @param world points are the points in world coordinates (that is: 3D-Space) that should be converted to Coordinates
	 * @return Coordinates (lat, long)
	 */
	std::vector<Coordinate> getGlobeCoordinates(std::vector<Vector3d> worldPoints);
	/**
	 * get Coordinates from multipe worldPoints of the Map
	 * @param world points (that is: 3D-Space) are the points in world coordinates that should be converted to Coordinates
	 * @return Coordinates in latitude and longitude
	 */
	std::vector<Coordinate> getPlaneCoordinates(std::vector<Vector3d> worldPoints);

	/**
	 * get the visible center of the globe/map
	 * @param carmeraPosition actual position of the camera
	 * @return Coordinate in latitude and longitude at the centre of the view
	 */
	Coordinate getCenterGlobeCoordinate(Vector3d cameraPosition);
	/**
	 * get intersection points of the view frustum with the visible globe/map
	 * @param planes of the view frustum as you can get them from VTK
	 * @param carmeraPosition actual position of the camera
	 * @return the four intersection points with the far plane in world coodinates
	 */
	std::vector<Vector3d> getIntersectionPoints(double planes[24], Vector3d cameraPosition);

private:
	///SphereTree to caluclate Intersection points with map
	vtkSmartPointer<vtkOBBTree> mySphereTree;
	///PlaneTree to caluclate Intersection points with globe
	vtkSmartPointer<vtkOBBTree> myPlaneTree;

	/**
	 * get intersection point of the view frustum with the visible globe/map
	 * @param 3 planes of the view frustum that should be cut (intersection point)
	 * @param carmeraPosition actual position of the camera
	 * @return returns the cut point in world coodinates
	 */
	Vector3d getIntersectionPoint(double plane1[4], double plane2[4], double plane3[4],
	                              Vector3d cameraPosition);
	/**
	 * calculate intersection point of three planes
	 * @param 3 planes of the view frustum that should be cut (intersection point)
	 *          planes must be given as ax + by + cz + d = 0
	 * @return return the intersection point
	 */
	Vector3d cutPlanes(double planes[3][4]);
	vtkSmartPointer<vtkOBBTree> getOBBTree();

	/**
	 * Get the Coordinate from a point at the globe (in world coordinates)
	 * @param point globe point in world coordinates
	 * @return Coordinate in latitude and longitude
	 */
	Coordinate getCoordinatesFromGlobePoint(Vector3d point);
	/**
	 * Get the Coordinate from a point at the map (in world coordinates)
	 * @param point map point in world coordinates
	 * @return Coordinate in latitude and longitude
	 */
	Coordinate getCoordinatesFromPlanePoint(Vector2d point);

	/**
	 * Returns the index of the specified tile within the tile array.
	 */
	unsigned int getTileIndex(int lon, int lat) const;

	void createTiles();
	void createOBBTrees();

	void onTileLoad(ImageTile tile);

	vtkRenderer& myRenderer;

	vtkSmartPointer<vtkPlaneSource> myPlaneSource;
	vtkSmartPointer<vtkPolyDataMapper> myPlaneMapper;

	ImageDownloader myDownloader;

	std::vector<std::unique_ptr<GlobeTile> > myTiles;

	unsigned int myZoomLevel;

	float myDisplayModeInterpolation;

	std::atomic_flag myIsClean;
};

#endif
