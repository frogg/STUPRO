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
    
    
    Coordinate getCoordinatesFromGlobePoint(double point[]);
    Coordinate getCoordinatesFromPlanePoint(double x, double y);

    std::vector<Coordinate> getGlobeCoordinates(std::vector<Vector3d> worldPoints);
    std::vector<Coordinate> getPlaneCoordinates(std::vector<Vector3d> worldPoints);
    Coordinate getCenterGlobeCoordinate(double cameraPosition[]);
    std::vector<Vector3d> getIntersectionPoints(double planes[24], double cameraPosition[3]);
    

private:
    vtkSmartPointer<vtkOBBTree> mySphereTree;
    vtkSmartPointer<vtkOBBTree> myPlaneTree;
    void getIntersectionPoint(double plane1[4], double plane2[4], double plane3[4], double cameraPosition[], double intersection[3]);
    void cutPlanes(double planes[3][4], double cut [3]);
    vtkSmartPointer<vtkOBBTree> getOBBTree();
    
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
