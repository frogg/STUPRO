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

vtkStandardNewMacro(StuproInteractor);

StuproInteractor *StuproInteractor::New(vtkPVStuproView *application)
{
	StuproInteractor *retval = new StuproInteractor();
	retval->myVtkPVStuproView = application;
	return retval;
}

void StuproInteractor::OnMouseWheelForward()
{
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
							this->Interactor->GetEventPosition()[1]);

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	// TODO: Set better min-distance.
	if(camera->GetDistance() >= myVtkPVStuproView->getGlobeRadius() * 1.75f)
	{
		zoomWithFactor(120);
	}
}

void StuproInteractor::OnMouseWheelBackward()
{
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
							this->Interactor->GetEventPosition()[1]);

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	if(camera->GetDistance() <= myVtkPVStuproView->getGlobeRadius() * 8.f)
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

	// Determine the factor depending on the camera center
	// to be used for zooming in/out.
	double dyf = factor / center[1];
	double zoomFactor = pow(1.1, dyf);

	camera->Dolly(zoomFactor);

	if(rwi->GetLightFollowCamera())
	{
		this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
	}

	rwi->Render();
}

void StuproInteractor::OnMiddleButtonDown()
{
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
							this->Interactor->GetEventPosition()[1]);

	if(this->CurrentRenderer == NULL)
	{
		return;
	}

	if(myVtkPVStuproView->getDisplayMode() == vtkPVStuproView::DisplayMode::DisplayMap)
	{
		this->GrabFocus(this->EventCallbackCommand);
		this->StartPan();
	}
}

void StuproInteractor::OnMiddleButtonUp()
{
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
							this->Interactor->GetEventPosition()[1]);

	switch(this->State)
	{
	case VTKIS_PAN:
		this->EndPan();
		if(this->Interactor)
		{
			this->ReleaseFocus();
		}
		break;
	}
}

void StuproInteractor::OnRightButtonDown()
{
	OnLeftButtonDown();
}

void StuproInteractor::OnRightButtonUp()
{
	OnLeftButtonUp();
}

/**
 * Basically a copy of the internal Rotate()-function.
 * Override function to prevent rotating the DisplayMap around the y-axis.
 */
void StuproInteractor::Rotate()
{
	if(this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	vtkRenderWindowInteractor *rwi = this->Interactor;

	// Slows the rotation down when closer to the globe/map
	double disFac = 1 / sqrt(camera->GetDistance() * 5 / 2);
	double distanceFactor = pow(1 - disFac, 2);

	int dx = 0;

	//Set the horizontal rotation only if in DisplayGlobe.
	if(myVtkPVStuproView->getDisplayMode() == vtkPVStuproView::DisplayMode::DisplayGlobe)
	{
		dx = -(rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0])*distanceFactor;
	}

	int dy = -(rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1])*distanceFactor;

	int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();

	double a = dx / static_cast<double>(size[0]) * 180.0;
	double e = dy / static_cast<double>(size[1]) * 180.0;

	if(rwi->GetShiftKey())
	{
		if(abs(dx) >= abs(dy))
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
	if((angle + e) > 179.0 ||
	   (angle + e) < 1.0)
	{
		e = 0.0;
	}

	camera->Elevation(e);

	if(this->AutoAdjustCameraClippingRange)
	{
		this->CurrentRenderer->ResetCameraClippingRange();
	}

	rwi->Render();
}
