#include <StuproInteractor.hpp>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkInteractorStyle.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkPVStuproView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <cmath>
#include <cstdlib>

// Needs to be created for vtk to register this Class.
vtkStandardNewMacro(StuproInteractor)


StuproInteractor::StuproInteractor() {

	zoomFactor = 120;
	//Zoom out when initialized.
	//zoomWithFactor(-(zoomFactor * 15));
}



StuproInteractor *StuproInteractor::New(vtkPVStuproView *application)
{
	StuproInteractor *retval = new StuproInteractor();
	retval->myVtkPVStuproView = application;

	return retval;
}

// One RenderWindow can have multiple renderers, we need to determine
// the one we clicked via our mouse position.
void StuproInteractor::setCurrentRendererViaPosition()
{
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
		this->Interactor->GetEventPosition()[1]);
}

void StuproInteractor::OnMouseWheelForward()
{
	setCurrentRendererViaPosition();

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	// TODO: Set better min-distance.
	if (camera->GetDistance() >= myVtkPVStuproView->getGlobeRadius() * 1.75f)
	{
		zoomWithFactor(zoomFactor);
	}
}

void StuproInteractor::OnMouseWheelBackward()
{
	setCurrentRendererViaPosition();

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	if (camera->GetDistance() <= myVtkPVStuproView->getGlobeRadius() * 8.f)
	{
		zoomWithFactor(-(zoomFactor));
	}
}

void StuproInteractor::zoomWithFactor(float factor)
{
	setCurrentRendererViaPosition();

	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;
	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	double *center = this->CurrentRenderer->GetCenter();

	// Determine the factor depending on the camera center
	// to be used for zooming in/out.
	double dyf = factor / center[1];
	double zoomFactor = pow(1.1, dyf);

	camera->Dolly(zoomFactor);

	if (rwi->GetLightFollowCamera())
	{
		this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
	}

	rwi->Render();
}

void StuproInteractor::OnMiddleButtonDown()
{
	setCurrentRendererViaPosition();

	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	if (myVtkPVStuproView->getDisplayMode() == vtkPVStuproView::DisplayMode::DisplayMap)
	{
		this->GrabFocus(this->EventCallbackCommand);
		this->StartPan();
	}
}

void StuproInteractor::OnMiddleButtonUp()
{
	setCurrentRendererViaPosition();

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

void StuproInteractor::OnRightButtonDown()
{
	//TODO: Implement independent cameraMovement from Globe in order to
	//		tilt the camera
	OnLeftButtonDown();
}

void StuproInteractor::OnRightButtonUp()
{
	//TODO: Implement independent cameraMovement from Globe in order to
	//		tilt the camera
	OnLeftButtonUp();
}

/**
* Basically a copy of the internal Rotate()-function.
* Override function to prevent rotating the DisplayMap around the y-axis.
*/
void StuproInteractor::Rotate()
{
	
	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	vtkRenderWindowInteractor *rwi = this->Interactor;

	// Slows the rotation down when closer to the globe/map
	double distanceFactor = pow(1 - (1 / sqrt(camera->GetDistance() * 5 / 2)), 2);

	int dx = 0;

	//Set the horizontal rotation only if in DisplayGlobe.
	if (myVtkPVStuproView->getDisplayMode() == vtkPVStuproView::DisplayMode::DisplayGlobe)
	{
		dx = -(rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0])*distanceFactor;
	}

	int dy = -(rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1])*distanceFactor;

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