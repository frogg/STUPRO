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
	registerTimerCallback();
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
    // Create callback function that updates the display mode interpolation value.
    auto timerFunc = [](vtkObject* caller, unsigned long eventId, void* clientData, void* callData)
    {
        // Capture the client using the void* of the arguments-list.
        vtkPVStuproView& client = *static_cast<vtkPVStuproView*>(clientData);

        // Determine target interpolation based on display mode.
        // If the current mode is the globe, try and switch to the map and vice versa.
        float interpolationTarget = client.displayMode == DisplayGlobe ? static_cast<float>(DisplayMap) : static_cast<float>(DisplayMap);

        // Check if change is significant enough to be re-rendered.
        if(std::abs(interpolationTarget - client.displayModeInterpolation) > 0.000001f)
        {
            // Controls the speed of the globe-map transition.
            float effectSpeed = Configuration::getInstance().getFloat("globe.transitionEffectSpeed");

            // Smoothly transition interpolation value based on previous and target value.
            client.globe->setDisplayModeInterpolation((interpolationTarget * effectSpeed +
                                                       client.globe->getDisplayModeInterpolation()) / (effectSpeed + 1.f));

            // Update renderer.
            client.GetRenderWindow()->Render();
        }
    };

    // Create and assign callback for the interpolation function.
    vtkSmartPointer<vtkCallbackCommand> timerCallback = vtkSmartPointer<vtkCallbackCommand>::New();
    timerCallback->SetCallback(timerFunc);
    timerCallback->SetClientData(this);

    // Workaround for a VTK bug involving a missing Xt App Context causing a segfault in
    // CreateRepeatingTimer.
    GetRenderWindow()->Render();

    // Enable timer on the render window.
    // TODO: Change this to a Qt-based timer system.
    GetRenderWindow()->GetInteractor()->CreateRepeatingTimer(17);
    GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::TimerEvent, timerCallback);
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


void vtkPVStuproView::moveCamera(float latitude, float longitude)
{
	this->moveCamera(latitude, longitude, this->getCameraDistance());
}

void vtkPVStuproView::moveCamera(float latitude, float longitude, float distance)
{
	// left-right, up-down, close-far
	Vector3d position(0.0, 0.0, 2.6);
	
	// probably important for map view
	Vector3d focus(0.0, 0.0, 0.0);

	if (this->displayMode == DisplayGlobe) {
		float lat = KRONOS_PI * latitude / 180;
		float lon = KRONOS_PI * longitude / 180;
		position.x = distance * cosf(lat) * sinf(lon);
		position.y = distance * sinf(lat);
		position.z = distance * cosf(lat) * cosf(lon);
	} else {
		// TODO: calculate the actual camera coordinates for the map view
	}

	vtkCamera* camera = GetActiveCamera();

	camera->SetPosition(position.array());
	camera->SetFocalPoint(focus.array());

	GetRenderWindow()->Render();
}

float vtkPVStuproView::getCameraDistance()
{
	vtkCamera* cam = GetActiveCamera();
	double x, y, z;
	cam->GetPosition(x, y, z);
	return sqrt(
		x * x +
		y * y +
		z * z
	);
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
