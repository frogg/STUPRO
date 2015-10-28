#ifndef STUPRO_GLOBE_HPP
#define STUPRO_GLOBE_HPP

#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkPlaneSource.h>
#include <vtkRenderWindow.h>
#include "Vector2.hpp"

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

private:
	
	vtkRenderWindow & myRenderWindow;
	
	vtkSmartPointer<vtkPlaneSource> myPlaneSource;
	vtkSmartPointer<vtkPolyDataMapper> myPlaneMapper;
};

#endif
