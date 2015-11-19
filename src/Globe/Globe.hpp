#ifndef STUPRO_GLOBE_HPP
#define STUPRO_GLOBE_HPP

#include <Utils/Math/Vector2.hpp>
#include <Utils/Math/Vector3.hpp>
#include <Utils/TileDownload/ImageDownloader.hpp>
#include <Utils/Math/IncludeEigen.hpp>
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
typedef Vector2d Coordinate;

class Globe
{
public:
	Globe(vtkRenderer & renderer);
	virtual ~Globe();

	void setResolution(Vector2u resolution);
	Vector2u getResolution() const;

	vtkSmartPointer<vtkPolyDataMapper> getPlaneMapper() const;
	
	vtkRenderWindow & getRenderWindow() const;
	vtkRenderer & getRenderer() const;
	
	void setZoomLevel(unsigned int zoomLevel);
	unsigned int getZoomLevel() const;
	
	GlobeTile & getTileAt(int lon, int lat) const;
		
	void setDisplayModeInterpolation(float displayMode);
	float getDisplayModeInterpolation() const;
	
	bool checkDirty();
    

    /**
     * get Coordinates (lat, long) from multipe worldPoints of the Globe
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
    Vector3d getIntersectionPoint(double plane1[4], double plane2[4], double plane3[4], Vector3d cameraPosition);
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

    
	unsigned int getTileIndex(int lon, int lat) const;
	
	void createTiles();
    void createOBBTrees();
	
	void onTileLoad(ImageTile tile);
	
	vtkRenderer & myRenderer;
	
	vtkSmartPointer<vtkPlaneSource> myPlaneSource;
	vtkSmartPointer<vtkPolyDataMapper> myPlaneMapper;
	
	ImageDownloader myDownloader;
	
	std::vector<std::unique_ptr<GlobeTile> > myTiles;
		
	unsigned int myZoomLevel;
	
	float myDisplayModeInterpolation;
	
	std::atomic_flag myIsClean;
};

#endif
