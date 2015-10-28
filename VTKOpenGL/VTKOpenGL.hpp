#ifndef STUPRO_VTKOPENGL_HPP
#define STUPRO_VTKOPENGL_HPP

#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkShader2.h>
#include <vtkUniformVariables.h>
#include <vtkOpenGLTexture.h>
#include "vtkOBBTree.h"

#include <functional>
#include <cmath>
#include <string>

#include "StuproInteractor.hpp"

struct Coordinate{
        double latitude;
        double longitude;
};

class VTKOpenGL
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
	void initShaders();
	void initCallbacks();

	vtkSmartPointer<vtkOpenGLTexture> loadAlphaTexture(std::string rgbFile,
	        std::string alphaFile) const;

	vtkSmartPointer<vtkRenderer> myRenderer;
	vtkSmartPointer<vtkRenderWindow> myRenderWindow;
	vtkSmartPointer<vtkActor> myPlaneActor;

	vtkSmartPointer<vtkShader2> myVertexShader;
	vtkSmartPointer<vtkShader2> myFragmentShader;
    vtkSmartPointer<vtkOBBTree> myTree;
    
	DisplayMode myDisplayMode;

    static Coordinate getCoordinates(double point[]);
	float myGlobeRadius;
	float myPlaneSize;
	float myDisplayModeInterpolation;
	float myHeightFactor;
};


#endif
