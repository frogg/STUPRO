#include <Globe/Globe.hpp>
#include <StuproInteractor.hpp>
#include <Utils/Misc/MakeUnique.hpp>
#include <Utils/Config/Configuration.hpp>

#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkIndent.h>
#include <vtkInteractorStyle.h>
#include <vtkObjectFactory.h>
#include <vtkPVStuproView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <cmath>

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
	// Set fitting clipping range.
	float r1 = this->globeRadius * 100.f;
	float r2 = 0.001f;
	this->GetRenderer()->ResetCameraClippingRange(r1, r2, r1, r2, r1, r2);
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
