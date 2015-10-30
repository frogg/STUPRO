#include "StuproInteractor.hpp"
#include "vtkRenderWindowInteractor.h"
#include "vtkCallbackCommand.h"
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>

#include <vtkObjectFactory.h>

vtkStandardNewMacro(StuproInteractor);


void StuproInteractor::OnMouseWheelForward() {
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
		this->Interactor->GetEventPosition()[1]);
	
	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	if (camera->GetDistance() >= 0.8){
		zoomWithFactor(120);
	}
}

void StuproInteractor::OnMouseWheelBackward() {
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
		this->Interactor->GetEventPosition()[1]);

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	if (camera->GetDistance() <= 5){
		zoomWithFactor(-120);
	}
}

void StuproInteractor::zoomWithFactor(float factor)
{
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
		this->Interactor->GetEventPosition()[1]);

	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;
	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	double *center = this->CurrentRenderer->GetCenter();

	double dyf = factor / center[1];
	double zoomFactor = pow(1.1, dyf);

	camera->Dolly(zoomFactor);

	if (rwi->GetLightFollowCamera())
	{
		this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
	}

	rwi->Render();
}
