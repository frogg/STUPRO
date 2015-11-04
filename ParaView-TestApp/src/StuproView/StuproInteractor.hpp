#ifndef STUPRO_STUPROINTERACTOR_HPP
#define STUPRO_STUPROINTERACTOR_HPP

#include <vtkInteractorStyleTerrain.h>
#include <cstring>
#include "vtkPVStuproView.h"

class StuproInteractor : public vtkInteractorStyleTerrain
{
public:
	static StuproInteractor *New(vtkPVStuproView *application);
	static StuproInteractor *New();

	vtkTypeMacro(StuproInteractor, vtkInteractorStyleTerrain);

	void OnTimer() override
	{
	}

	
	void OnMiddleButtonDown() override;

	void OnMiddleButtonUp() override;

	void OnRightButtonDown() override;

	void OnRightButtonUp() override;
	

	void OnMouseWheelForward() override;

	void OnMouseWheelBackward() override;

	void zoomWithFactor(float factor);

	void Rotate() override;

private:
	vtkPVStuproView* myVtkPVStuproView = NULL;
};

#endif
