/*
 * Globe.cpp
 *
 *  Created on: Oct 23, 2015
 *      Author: bbq
 */

#include "Globe.hpp"

Globe::Globe(vtkRenderWindow & renderWindow) :
		myRenderWindow(renderWindow),
		myZoomLevel(0)
{
	myPlaneSource = vtkPlaneSource::New();
	
	myPlaneMapper = vtkPolyDataMapper::New();
	myPlaneMapper->SetInputConnection(myPlaneSource->GetOutputPort());
}

Globe::~Globe()
{
}

void Globe::setResolution(Vector2u resolution)
{
	myPlaneSource->SetResolution(resolution.x, resolution.y);
}

Vector2u Globe::getResolution() const
{
	Vector2i ret;
	myPlaneSource->GetResolution(ret.x, ret.y);
	return Vector2u(ret);
}

vtkSmartPointer<vtkPolyDataMapper> Globe::getPlaneMapper() const
{
	return myPlaneMapper;
}

vtkRenderWindow& Globe::getRenderWindow() const
{
	return myRenderWindow;
}

void Globe::setZoomLevel(unsigned int zoomLevel)
{
	myZoomLevel = 0;
}

unsigned int Globe::getZoomLevel() const
{
	return myZoomLevel;
}
