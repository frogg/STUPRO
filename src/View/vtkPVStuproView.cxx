#include <Kronos.h>
#include <Globe/Globe.hpp>
#include <StuproInteractor.hpp>
#include <Utils/Misc/KronosLogger.hpp>
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
#include <vector>

vtkStandardNewMacro(vtkPVStuproView);

vtkPVStuproView::vtkPVStuproView()
{
}

vtkPVStuproView::~vtkPVStuproView()
{
}

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

	// Create interactor for render window.
//	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
//	interactor->SetRenderWindow(this->GetRenderWindow());

	// Create interactor style for render window.
//vtkSmartPointer<StuproInteractor> interactorStyle = StuproInteractor::New(this);
//	interactorStyle->SetAutoAdjustCameraClippingRange(false);
//	interactor->SetInteractorStyle(interactorStyle);

	// Zoom out.
	this->GetRenderWindow()->Render();
	//interactorStyle->zoomWithFactor(-1800.f);
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
    GetRenderWindow()->GetInteractor()->CreateRepeatingTimer(17);
    GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::TimerEvent, timerCallback);
}

void vtkPVStuproView::initGlobe()
{
    // Initialize a unique pointer with a new instance of the Globe
    // using the current renderer.
    this->globe = makeUnique<Globe>(*this->GetRenderer());
}

void vtkPVStuproView::moveCamera(float latitude, float longitude)
{
	this->moveCamera(latitude, longitude, this->getCameraDistance());
}

void vtkPVStuproView::moveCamera(float latitude, float longitude, float distance)
{
	float position[3] = { 0.0, 0.0, 2.6 }; // left-right, up-down, close-far
	float focus[3] = { 0 }; // probably important for map view

	// if (this->displayMode == DisplayGlobe) {
		float lat = KRONOS_PI * latitude / 180;
		float lon = KRONOS_PI * longitude / 180;
		position[0] = distance * cosf(lat) * sinf(lon);
		position[1] = distance * sinf(lat);
		position[2] = distance * cosf(lat) * cosf(lon);
	// } else {
		// TODO: calculate the actual camera coordinates for the map view
	// }

	vtkCamera* camera = GetActiveCamera();

	camera->SetPosition(position[0], position[1], position[2]);
	camera->SetFocalPoint(focus[0], focus[1], focus[2]);

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
