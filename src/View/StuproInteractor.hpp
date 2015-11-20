#ifndef STUPRO_STUPROINTERACTOR_HPP
#define STUPRO_STUPROINTERACTOR_HPP

#include <stddef.h>
#include <vtkInteractorStyleTerrain.h>
#include <vtkSetGet.h>
#include <utils\Math\Vector2.hpp>
#include <vtkCamera.h>

class vtkPVStuproView;

class StuproInteractor : public vtkInteractorStyleTerrain
{
public:
	StuproInteractor();

	static StuproInteractor *New();
	/**
	* Constructor needed for keeping track of the globe-radius.
	*
	* @param vtkPVStuproView
	*/
	static StuproInteractor *New(vtkPVStuproView *application);

	vtkTypeMacro(StuproInteractor, vtkInteractorStyleTerrain)

	/**
	* Overrides the inner unnecessary OnTimer Event
	* needed for our vtkCommand::TimerEvent in vtkPVStuproView.cxx
	*/
	void OnTimer() override
	{
		// Empty on purpose.
	}

	/**
	* One RenderWindow can have multiple renderers, we need to determine
	* the one we clicked via our mouse position.
	*/
	void setCurrentRendererViaPosition();

	/**
	* Moving the map around.
	*/
	void OnMiddleButtonDown() override;
	void OnMiddleButtonUp() override;

	/**
	* TODO camera-tilt
	*/
	void OnRightButtonDown() override;
	void OnRightButtonUp() override;

	/**
	* Zooms in/out of the globe.
	*/
	void OnMouseWheelForward() override;
	void OnMouseWheelBackward() override;

	/**
	* Rotates the globe, locking the y-axis, if DisplayMode=DisplayMap.
	* Rotationspeed is depending on distance to globe
	*/
	void Rotate() override;

private:
	vtkPVStuproView* myVtkPVStuproView = NULL;
	/**
	* Zooms in or out with regards to the current camera centerposition.
	* factor > 0: Zoom in, factor < 0: Zoom out
	*/
	void zoomWithFactor(float factor);

	static const double distanceOptimization;
	/**
	* sets the Rotationspeed, depending on Camera-Distance
	*
	* @return a vector with the horizontal and vertical deltas 
	*/
	Vector2d calculateRotationParameters();
	
	/**
	* sets the ViewingAngle
	*
	* @param the horizontal and vertical deltas
	* @return a Vector with the azimuth and elevation
	*/
	Vector2d calculateCameraRotators(Vector2d rotationParameters);
	
	float zoomFactor;
};

#endif