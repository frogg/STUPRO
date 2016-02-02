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
#include <vtkRenderWindowInteractor.h>
#include <vtkMatrix4x4.h>

vtkStandardNewMacro(vtkPVStuproView);

void vtkPVStuproView::Initialize(unsigned int id)
{
	this->Superclass::Initialize(id);

	initParameters();
	initRenderer();
	//registerTimerCallback();
	initGlobe();
}

void vtkPVStuproView::initParameters()
{
	// Initialize parameters.
	this->displayMode = DisplayGlobe;
}

void vtkPVStuproView::initRenderer()
{
	this->cameraModifiedCallback = vtkCallbackCommand::New();
	
    this->cameraModifiedCallback->SetCallback(
		[](vtkObject* object, unsigned long eid, void* clientdata, void *calldata)
		{
			vtkPVStuproView * view = (vtkPVStuproView *)clientdata;
			view->getGlobe()->onCameraChanged();
		});
    
    this->cameraModifiedCallback->SetClientData(this);
	
	this->GetRenderer()->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::InteractionEvent,
		this->cameraModifiedCallback);
}

void vtkPVStuproView::registerTimerCallback()
{
}

void vtkPVStuproView::initGlobe()
{
	const Configuration & config = Configuration::getInstance();
	
	GlobeConfig globeConfig;
	
	globeConfig.globeRadius = config.getFloat("globe.globeRadius");
	globeConfig.flatMapSize.x = config.getFloat("globe.flatMapSize.width");
	globeConfig.flatMapSize.y = config.getFloat("globe.flatMapSize.height");
	globeConfig.internalPlaneSize = config.getFloat("globe.internalPlaneSize");
	globeConfig.earthRadius = config.getFloat("globe.earthRadius");
	globeConfig.heightFactor = config.getFloat("globe.heightFactor");
	
	// Initialize a unique pointer with a new instance of the Globe
	// using the current renderer.
	this->globe = makeUnique<Globe>(*this->GetRenderer(), globeConfig);
}

void vtkPVStuproView::switchCurrentDisplayMode()
{
	// Invert the display mode and set the interpolation using a static cast.
	this->displayMode = this->displayMode == DisplayGlobe ? DisplayMap : DisplayGlobe;
	this->globe->setDisplayModeInterpolation(static_cast<float>(this->displayMode));

	// Render the view again.
	GetRenderWindow()->Render();
}

vtkPVStuproView::DisplayMode vtkPVStuproView::getDisplayMode() const
{
	return this->displayMode;
}

Globe * vtkPVStuproView::getGlobe() const
{
	return this->globe.get();
}
