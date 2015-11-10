#ifndef STUPRO_GLOBE_HPP
#define STUPRO_GLOBE_HPP

#include "ImageDownloader.hpp"
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <memory>
#include <atomic>
#include <vector>
#include "vtkOBBTree.h"
#include "Coordinate.hpp"

#include "Vector2.hpp"
#include "Vector3.hpp"

class GlobeTile;

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
	
	float getPlaneSize() const;
	
	void setDisplayModeInterpolation(float displayMode);
	float getDisplayModeInterpolation() const;
	
	bool checkDirty();
    
    static void cutPlanes(double planes[3][4], double cut [3]);
    static void getIntersectionPoint(double plane1[4], double plane2[4], double plane3[4], double cameraPosition[],vtkSmartPointer<vtkOBBTree> tree, double intersection[3]);
    
    static Coordinate getCenterGlobeCoordinate(vtkSmartPointer<vtkOBBTree> tree, double cameraPosition[], double globeRadius);
    
    static std::vector<Vector3d> getIntersectionPoints(double planes[24], double cameraPosition[3], vtkSmartPointer<vtkOBBTree> tree);
    static std::vector<Coordinate> getGlobeCoordinates(std::vector<double*> worldPoints, double radius);
    static std::vector<Coordinate> getPlaneCoordinates(std::vector<double*> worldPoints, double planeWidth, double planeHeight);

private:
	
	unsigned int getTileIndex(int lon, int lat) const;
	
	void createTiles();
	
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
