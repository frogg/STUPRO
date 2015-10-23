#ifndef STUPRO_GLOBETILE_HPP
#define STUPRO_GLOBETILE_HPP

#include "Rect.hpp"
#include "Vector3.hpp"

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkTexture.h>
#include <vtkPlaneSource.h>
#include <vtkShader2.h>

class GlobeTile
{
public:

	GlobeTile();

	void setBounds(RectF bounds);
	RectF getBounds() const;

	void setTexture(vtkSmartPointer<vtkTexture> texture);
	vtkSmartPointer<vtkTexture> getTexture() const;
	
	void setLowerHeight(float lower);
	float getLowerHeight() const;
	
	void setUpperHeight(float upper);
	float getUpperHeight() const;

	vtkSmartPointer<vtkActor> getActor() const;

private:

	RectF myBounds;

	Vector2u myResolution;

	vtkSmartPointer<vtkActor> myActor;
	vtkSmartPointer<vtkTexture> myTexture;
	vtkSmartPointer<vtkPlaneSource> myPlaneSource;
	vtkSmartPointer<vtkShader2> myVertexShader;
	vtkSmartPointer<vtkShader2> myFragmentShader;

};

#endif
