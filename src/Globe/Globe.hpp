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
     * @param world points are the points in world coordinates that should be converted to Coordinates
     * @return Coordinates (lat, long)
     */
    std::vector<Coordinate> getGlobeCoordinates(std::vector<Vector3d> worldPoints);
    /**
     * get Coordinates (lat, long) from multipe worldPoints of the Map
     * @param world points are the points in world coordinates that should be converted to Coordinates
     * @return Coordinates (lat, long)
     */
    std::vector<Coordinate> getPlaneCoordinates(std::vector<Vector3d> worldPoints);
    
    /**
     * get the visible center of the globe/map
     * @param carmeraPosition actual position of the camera
     * @return Coordinate (lat, long) at the centre of the view
     */
    Coordinate getCenterGlobeCoordinate(double cameraPosition[]);
    /**
     * get intersection points of the view frustum with the visible globe/map
     * @param planes of the view frustum
     * @param carmeraPosition actual position of the camera
     * @return 4 edge points in world coodinates
     */
    std::vector<Vector3d> getIntersectionPoints(double planes[24], double cameraPosition[3]);
    

private:
    ///SphereTree to caluclate Intersection points with map
    vtkSmartPointer<vtkOBBTree> mySphereTree;
    ///PlaneTree to caluclate Intersection points with globe
    vtkSmartPointer<vtkOBBTree> myPlaneTree;
    
    /**
     * get intersection point of the view frustum with the visible globe/map
     * @param 3 planes of the view frustum that should be cut (intersection point)
     * @param carmeraPosition actual position of the camera
     * @param intersection[3], returns 1 edge points in world coodinates
     */
    void getIntersectionPoint(double plane1[4], double plane2[4], double plane3[4], double cameraPosition[], double intersection[3]);
    /**
     * calculate intersection point of three planes
     * @param 3 planes of the view frustum that should be cut (intersection point)
     * @param cut, return the intersection point
     */
    void cutPlanes(double planes[3][4], double cut [3]);
    vtkSmartPointer<vtkOBBTree> getOBBTree();
    
    /**
     * Get the Coordinate(lat, long) from a point at the globe (in world coordinates)
     * @param point globe point in world coordinates
     * @return Coordinate (lat, long)
     */
    Coordinate getCoordinatesFromGlobePoint(double point[]);
    /**
     * Get the Coordinate(lat, long) from a point at the map (in world coordinates)
     * @param point map point in world coordinates
     * @return Coordinate (lat, long)
     */
    Coordinate getCoordinatesFromPlanePoint(double x, double y);

    
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
