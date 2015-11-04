#include "StuproInteractor.hpp"
#include "vtkRenderWindowInteractor.h"
#include "vtkCallbackCommand.h"
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <string.h>
#include <vtkMath.h>

#include <vtkObjectFactory.h>

StuproInteractor *StuproInteractor::New(vtkPVStuproView *application){
	StuproInteractor *retval = new StuproInteractor();
	retval->myVtkPVStuproView = application;
	return retval;
}

void StuproInteractor::OnMouseWheelForward()
{
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
							this->Interactor->GetEventPosition()[1]);

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	if(camera->GetDistance() >= (myVtkPVStuproView->getGlobeRadius())+0.5)
	{
		zoomWithFactor(120);
	}
}

void StuproInteractor::OnMouseWheelBackward()
{
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
							this->Interactor->GetEventPosition()[1]);

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	if(camera->GetDistance() <= (myVtkPVStuproView->getGlobeRadius())+8)
	{
		zoomWithFactor(-120);
	}
}

void StuproInteractor::zoomWithFactor(float factor)
{
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
							this->Interactor->GetEventPosition()[1]);

	if(this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;
	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	double *center = this->CurrentRenderer->GetCenter();

	double dyf = factor / center[1];
	double zoomFactor = pow(1.1, dyf);

	camera->Dolly(zoomFactor);

	if(rwi->GetLightFollowCamera())
	{
		this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
	}

	rwi->Render();
}

void StuproInteractor::OnMiddleButtonDown() {
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
		this->Interactor->GetEventPosition()[1]);

	if (myVtkPVStuproView->getDisplayMode() == vtkPVStuproView::DisplayMode::DisplayMap)
	{
		this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
			this->Interactor->GetEventPosition()[1]);
		if (this->CurrentRenderer == NULL)
		{
			return;
		}

		this->GrabFocus(this->EventCallbackCommand);
		this->StartPan();
	}
}

void StuproInteractor::OnMiddleButtonUp() {
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
		this->Interactor->GetEventPosition()[1]);

	switch (this->State)
	{
	case VTKIS_PAN:
		this->EndPan();
		if (this->Interactor)
		{
			this->ReleaseFocus();
		}
		break;
	}
}

void StuproInteractor::OnRightButtonDown() {
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
		this->Interactor->GetEventPosition()[1]);
	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	this->GrabFocus(this->EventCallbackCommand);
	this->StartRotate();
}

void StuproInteractor::OnRightButtonUp() {
	switch (this->State)
	{
	case VTKIS_ROTATE:
		this->EndRotate();
		if (this->Interactor)
		{
			this->ReleaseFocus();
		}
		break;
	}
}

/**
* overriden function disables horizontal rotation when in 2.5D mode
*/
void StuproInteractor::Rotate()
{
	if (this->CurrentRenderer == NULL)
	{
		return;
	}
	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	vtkRenderWindowInteractor *rwi = this->Interactor;

	// slows the rotation down, when closer to the globe/map
	double distanceFactor = pow(1 - 1 / sqrt(camera->GetDistance() * 5 / 2), 2);
	//cout << distanceFactor << endl;

	int dx;
	int dy;
	if (myVtkPVStuproView->getDisplayMode() == vtkPVStuproView::DisplayMode::DisplayMap){
		// disables horizontal rotation
		dx = 0;
	}
	else if (myVtkPVStuproView->getDisplayMode() == vtkPVStuproView::DisplayMode::DisplayGlobe){
		dx = -(rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0])*distanceFactor;
	}
	dy = -(rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1])*distanceFactor;

	int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();

	double a = dx / static_cast<double>(size[0]) * 180.0;
	double e = dy / static_cast<double>(size[1]) * 180.0;

	if (rwi->GetShiftKey())
	{
		if (abs(dx) >= abs(dy))
		{
			e = 0.0;
		}
		else
		{
			a = 0.0;
		}
	}


	camera->Azimuth(a);

	double dop[3], vup[3];

	camera->GetDirectionOfProjection(dop);
	vtkMath::Normalize(dop);
	camera->GetViewUp(vup);
	vtkMath::Normalize(vup);

	double angle = vtkMath::DegreesFromRadians(acos(vtkMath::Dot(dop, vup)));
	if ((angle + e) > 179.0 ||
		(angle + e) < 1.0)
	{
		e = 0.0;
	}

	camera->Elevation(e);

	if (this->AutoAdjustCameraClippingRange)
	{
		this->CurrentRenderer->ResetCameraClippingRange();
	}

	rwi->Render();
}

StuproInteractor* StuproInteractor::New()
{
	return new StuproInteractor;
}

/**
* Needed for vtk compatibility reasons
*/
//StuproInteractor* StuproInteractor::New() {
//	throw std::string("This method must not be called");
//}
