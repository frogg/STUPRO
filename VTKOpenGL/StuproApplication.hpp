#ifndef STUPRO_APPLICATION_HPP
#define STUPRO_APPLICATION_HPP

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <memory>

#include "Globe.hpp"

class Globe;

class StuproApplication
{
public:
	
	void run();

private:
	
	enum DisplayMode
	{
		DisplayGlobe, DisplayMap
	};

	void init();
	void initParameters();
	void initGlobe();
	void initRenderer();
	void initCallbacks();

	vtkSmartPointer<vtkRenderer> myRenderer;
	vtkSmartPointer<vtkRenderWindow> myRenderWindow;

	DisplayMode myDisplayMode;
	
	std::unique_ptr<Globe> myGlobe;

	float myGlobeRadius;
	float myPlaneSize;
	float myDisplayModeInterpolation;
	float myHeightFactor;
};

#endif
