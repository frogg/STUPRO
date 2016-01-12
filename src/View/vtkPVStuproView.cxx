#include <stddef.h>
#include <Utils/Config/Configuration.hpp>
#include <Utils/Misc/MakeUnique.hpp>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkInformation.h>
#include <vtkObjectFactory.h>
#include <vtkPVStuproView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkMatrix4x4.h>

vtkStandardNewMacro(vtkPVStuproView);

void vtkPVStuproView::Initialize(unsigned int id)
{
	this->Superclass::Initialize(id);

	initParameters();
	initRenderer();
	registerTimerCallback();
	initGlobe();
}

void vtkPVStuproView::initParameters()
{
	// Initialize parameters.
	this->displayMode = DisplayGlobe;
	this->globeRadius = Configuration::getInstance().getFloat("globe.radius");
	this->planeSize = Configuration::getInstance().getFloat("globe.planeSize");
	this->heightFactor = Configuration::getInstance().getFloat("globe.heightFactor");
}

void vtkPVStuproView::initRenderer()
{
	this->activeCameraCallback = vtkCallbackCommand::New();

	this->activeCameraCallback->SetCallback(
	        [](vtkObject* object, unsigned long eid, void* clientdata, void *calldata)
	        {
		        vtkPVStuproView * view = (vtkPVStuproView *)clientdata;

		        view->cameraModifiedCallback = vtkCallbackCommand::New();

		        view->cameraModifiedCallback->SetCallback(
				        [](vtkObject* object, unsigned long eid, void* clientdata, void *calldata)
				        {
					        vtkPVStuproView * view = (vtkPVStuproView *)clientdata;
					        view->getGlobe()->updateGlobeTileVisibility();
				        });
		        view->cameraModifiedCallback->SetClientData(clientdata);

		        vtkRenderer * renderer = vtkRenderer::SafeDownCast(object);

		        renderer->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, view->cameraModifiedCallback);
	        });
	this->activeCameraCallback->SetClientData(this);

	this->GetRenderer()->AddObserver(vtkCommand::ActiveCameraEvent, this->activeCameraCallback);
}

void vtkPVStuproView::registerTimerCallback()
{
}

void vtkPVStuproView::initGlobe()
{
	// Initialize a unique pointer with a new instance of the Globe
	// using the current renderer.
	this->globe = makeUnique<Globe>(*this->GetRenderer());
}

void vtkPVStuproView::switchCurrentDisplayMode()
{
	// TODO: Change this, for now resets the camera.
	GetActiveCamera()->SetPosition(0, 0, 2.8);
	GetActiveCamera()->SetFocalPoint(0, 0, 0);

	// Invert the display mode and set the interpolation using a static cast.
	this->displayMode = this->displayMode == DisplayGlobe ? DisplayMap : DisplayGlobe;
	this->globe->setDisplayModeInterpolation(static_cast<float>(this->displayMode));

	// Render the view again.
	GetRenderWindow()->Render();
}

float vtkPVStuproView::getGlobeRadius() const
{
	return this->globeRadius;
}

vtkPVStuproView::DisplayMode vtkPVStuproView::getDisplayMode() const
{
	return this->displayMode;
}

Globe * vtkPVStuproView::getGlobe() const
{
	return this->globe.get();
}
