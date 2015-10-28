#ifndef STUPRO_GLOBE_HPP
#define STUPRO_GLOBE_HPP

#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkPlaneSource.h>
#include <vtkRenderWindow.h>
#include <vector>

#include "GlobeTile.hpp"
#include "Vector2.hpp"

class Globe
{
public:
	Globe(vtkRenderWindow & renderWindow);
	virtual ~Globe();

	void setResolution(Vector2u resolution);
	Vector2u getResolution() const;

	vtkSmartPointer<vtkPolyDataMapper> getPlaneMapper() const;
	
	vtkRenderWindow & getRenderWindow() const;
	
	void setZoomLevel(unsigned int zoomLevel);
	unsigned int getZoomLevel() const;
	
	void getTileAt(int lon, int lat);

private:
	
	void createTiles();
	
	vtkRenderWindow & myRenderWindow;
	
	vtkSmartPointer<vtkPlaneSource> myPlaneSource;
	vtkSmartPointer<vtkPolyDataMapper> myPlaneMapper;
	
	std::vector<GlobeTile> myTiles;
	
	unsigned int myZoomLevel;
};

#endif
