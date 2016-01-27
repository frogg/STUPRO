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
#include <Globe/Globe.hpp>


// Needs to be created for vtk to register this Class.
vtkStandardNewMacro(StuproInteractor)


StuproInteractor::StuproInteractor() {

	zoomFactor = 120;
	//Zoom out when initialized.
	//zoomWithFactor(-(zoomFactor * 15));
}



StuproInteractor* StuproInteractor::New(vtkPVStuproView* application) {
	StuproInteractor* stuproInteracctor = new StuproInteractor();
	stuproInteracctor->myVtkPVStuproView = application;

	return stuproInteracctor;
}

// One RenderWindow can have multiple renderers, we need to determine
// the one we clicked via our mouse position.
void StuproInteractor::setCurrentRendererViaPosition() {
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
	                        this->Interactor->GetEventPosition()[1]);
}

void StuproInteractor::OnMouseWheelForward() {
	setCurrentRendererViaPosition();
	float minDistance = 2.75f;
	vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
	// TODO: Set better min-distance.
	if (camera->GetDistance() >= myVtkPVStuproView->getGlobeRadius() * minDistance) {
		zoomWithFactor(zoomFactor);
	}
}

void StuproInteractor::OnMouseWheelBackward() {
	setCurrentRendererViaPosition();
	float maxDistance = 8.0f;
	vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
	if (camera->GetDistance() <= myVtkPVStuproView->getGlobeRadius() * maxDistance) {
		zoomWithFactor(-(zoomFactor));
	}
}

void StuproInteractor::zoomWithFactor(float factor) {
	setCurrentRendererViaPosition();

	if (this->CurrentRenderer == NULL) {
		return;
	}

	vtkRenderWindowInteractor* renderWindowInteractor = this->Interactor;
	vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
	double* center = this->CurrentRenderer->GetCenter();

	// Determine the factor depending on the camera center
	// to be used for zooming in/out.
	double dyf = factor / center[1];
	double zoomFactor = pow(1.1, dyf);

	camera->Dolly(zoomFactor);

	if (renderWindowInteractor->GetLightFollowCamera()) {
		this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
	}


	if (this->myVtkPVStuproView->getGlobe()) {
		this->myVtkPVStuproView->getGlobe()->updateGlobeTileVisibility();
	}
	renderWindowInteractor->Render();

}

void StuproInteractor::OnMiddleButtonDown() {
	setCurrentRendererViaPosition();

	if (this->CurrentRenderer == NULL) {
		return;
	}

	if (myVtkPVStuproView->getDisplayMode() == vtkPVStuproView::DisplayMode::DisplayMap) {
		this->GrabFocus(this->EventCallbackCommand);
		this->StartPan();
	}
}

void StuproInteractor::OnMiddleButtonUp() {
	setCurrentRendererViaPosition();

	switch (this->State) {
	case VTKIS_PAN:
		this->EndPan();
		if (this->Interactor) {
			this->ReleaseFocus();
		}
		break;
	}
}

void StuproInteractor::OnRightButtonDown() {
	//TODO: Implement independent cameraMovement from Globe in order to
	//		tilt the camera
	OnLeftButtonDown();
}

void StuproInteractor::OnRightButtonUp() {
	//TODO: Implement independent cameraMovement from Globe in order to
	//		tilt the camera
	OnLeftButtonUp();
}


Vector2d StuproInteractor::calculateRotationParameters() {
	// Slows the rotation down when closer to the globe/map
	double distanceOptimization = 2.5;
	// TODO find a better solution
	double distanceFactor = pow(1 - (1 / sqrt(this->CurrentRenderer->GetActiveCamera()->GetDistance() *
	                                 distanceOptimization)), 2);
	//the horizontal and vertical rotation
	Vector2d rotationParameters(0,
	                            -(this->Interactor->GetEventPosition()[1] - this->Interactor->GetLastEventPosition()[1])
	                            *distanceFactor);

	//Set the horizontal rotation only if in DisplayGlobe.
	if (myVtkPVStuproView->getDisplayMode() == vtkPVStuproView::DisplayMode::DisplayGlobe) {
		rotationParameters.x = -(this->Interactor->GetEventPosition()[0] -
		                         this->Interactor->GetLastEventPosition()[0]) * distanceFactor;
	}
	return rotationParameters;
}


Vector2d StuproInteractor::calculateCameraRotators(Vector2d rotationParameters) {
	int* windowSize = this->CurrentRenderer->GetRenderWindow()->GetSize();
	double degrees = 180.0;
	// azimuth and elevation for the camera
	Vector2d cameraRotators(rotationParameters.x / static_cast<double>(windowSize[0]) * degrees,
	                        rotationParameters.y / static_cast<double>(windowSize[1]) * degrees);

	double directionOfProjection[3], viewUp[3];

	this->CurrentRenderer->GetActiveCamera()->GetDirectionOfProjection(directionOfProjection);
	vtkMath::Normalize(directionOfProjection);
	this->CurrentRenderer->GetActiveCamera()->GetViewUp(viewUp);
	vtkMath::Normalize(viewUp);

	// If we are exactly over the north / southpole we need to disable further elevation
	// otherwise the interactor starts jumping around
	double angle = vtkMath::DegreesFromRadians(acos(vtkMath::Dot(directionOfProjection, viewUp)));
	if ((angle + cameraRotators.y) > 179.0 ||
	        (angle + cameraRotators.y) < 1.0) {
		cameraRotators.y = 0.0;
	}
	return cameraRotators;
}



void StuproInteractor::Rotate() {
	// if there is no Renderer found, break
	if (this->CurrentRenderer == NULL) {
		return;
	}

	Vector2d rotationParameters = calculateRotationParameters();
	Vector2d cameraRotators = calculateCameraRotators(rotationParameters);

	vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
	camera->Azimuth(cameraRotators.x);
	camera->Elevation(cameraRotators.y);

	if (this->AutoAdjustCameraClippingRange) {
		this->CurrentRenderer->ResetCameraClippingRange();
	}

	if (this->myVtkPVStuproView->getGlobe()) {
		this->myVtkPVStuproView->getGlobe()->updateGlobeTileVisibility();
	}

	this->Interactor->Render();
}

