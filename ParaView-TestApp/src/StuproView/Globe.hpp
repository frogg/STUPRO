#ifndef STUPRO_GLOBE_HPP
#define STUPRO_GLOBE_HPP

#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <memory>
#include <vector>

#include "Vector2.hpp"

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

private:

	unsigned int getTileIndex(int lon, int lat) const;

	void createTiles();

	vtkRenderer & myRenderer;

	vtkSmartPointer<vtkPlaneSource> myPlaneSource;
	vtkSmartPointer<vtkPolyDataMapper> myPlaneMapper;

	std::vector<std::unique_ptr<GlobeTile> > myTiles;

	unsigned int myZoomLevel;

	float myDisplayModeInterpolation;
};

#endif
