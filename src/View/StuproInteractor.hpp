#ifndef STUPRO_STUPROINTERACTOR_HPP
#define STUPRO_STUPROINTERACTOR_HPP

#include <stddef.h>
#include <vtkInteractorStyleTerrain.h>
#include <vtkSetGet.h>

class vtkPVStuproView;

class StuproInteractor : public vtkInteractorStyleTerrain
{
public:
	StuproInteractor();

	static StuproInteractor *New();
	// Constructor needed for keeping track of the globe-radius.
	static StuproInteractor *New(vtkPVStuproView *application);

	vtkTypeMacro(StuproInteractor, vtkInteractorStyleTerrain)

	// Overrides the inner unnecessary OnTimer Event
	// needed for our vtkCommand::TimerEvent in vtkPVStuproView.cxx
	void OnTimer() override
	{
		// Empty on purpose.
	}

	void setCurrentRendererViaPosition();

	// Moving the globe/map around.
	void OnMiddleButtonDown() override;
	void OnMiddleButtonUp() override;

	// Rotates the globe/map for now.
	void OnRightButtonDown() override;
	void OnRightButtonUp() override;

	// Zooms in/out of the globe.
	void OnMouseWheelForward() override;
	void OnMouseWheelBackward() override;

	/**
	* Zooms in or out with regards to the current camera centerposition.
	* factor > 0: Zoom in, factor < 0: Zoom out
	*/
	

	// Rotates the globe, locking the y-axis, if DisplayMode=DisplayMap
	void Rotate() override;

private:
	vtkPVStuproView* myVtkPVStuproView = NULL;
	void zoomWithFactor(float factor);

	float zoomFactor;
};

#endif