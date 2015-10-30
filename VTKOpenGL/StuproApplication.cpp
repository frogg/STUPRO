#include "StuproApplication.hpp"

#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkInteractorStyle.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkWindow.h>
#include <cmath>

#include "MakeUnique.hpp"
#include "StuproInteractor.hpp"

void StuproApplication::run()
{
	// Perform initialization.
	init();

	// Update renderer.
	myRenderWindow->Render();

	// Start displaying!
	myRenderWindow->GetInteractor()->Start();
}

void StuproApplication::init()
{
	initParameters();
	initRenderer();
	initCallbacks();
	initGlobe();
}

void StuproApplication::initParameters()
{
	// Initialize parameters.
	myDisplayMode = DisplayGlobe;
	myGlobeRadius = 0.5f;
	myPlaneSize = 1.f;
	myHeightFactor = 0.05f;
}

void StuproApplication::initRenderer()
{
	// Create renderer with actor for the globe.
	myRenderer = vtkRenderer::New();

	// Set camera clipping range.
	float r = myGlobeRadius * 100.f;
	myRenderer->ResetCameraClippingRange(-r, r, -r, r, -r, r);

	// Create render window with renderer.
	myRenderWindow = vtkRenderWindow::New();
	myRenderWindow->SetWindowName("Famous Globe");
	myRenderWindow->AddRenderer(myRenderer);

	// Create interactor for render window.
	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<
	        vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(myRenderWindow);

	// Create interactor style for render window.
	vtkSmartPointer<vtkInteractorStyle> interactorStyle = StuproInteractor::New();
	interactorStyle->SetAutoAdjustCameraClippingRange(false);
	interactor->SetInteractorStyle(interactorStyle);
}

void StuproApplication::initCallbacks()
{
	// Create a callback function that updates the display mode interpolation value.
	auto timerFunc = [](vtkObject* caller, unsigned long eventId, void* clientData, void* callData)
	{
		StuproApplication & client = *((StuproApplication*)clientData);

		// Determine target interpolation based on display mode.
		    float interpolationTarget = client.myDisplayMode == DisplayGlobe ? 0.f : 1.f;

		    // Check if change is significant enough to be re-rendered.
		    if (std::abs(interpolationTarget - client.myGlobe->getDisplayModeInterpolation()) > 0.000001f)
		    {
			    // Controls the speed of the globe-map transition.
			    float effectSpeed = .2f;

			    // Smoothly transition interpolation value based on previous and target value.
			    client.myGlobe->setDisplayModeInterpolation((interpolationTarget * effectSpeed +
							    client.myGlobe->getDisplayModeInterpolation()) / (effectSpeed + 1.f));

			    // Update renderer.
			    client.myRenderWindow->Render();
		    }
	    };

	// Create and assign callback for clipping function.
	vtkSmartPointer<vtkCallbackCommand> timerCallback = vtkSmartPointer<vtkCallbackCommand>::New();
	timerCallback->SetCallback(timerFunc);
	timerCallback->SetClientData(this);

	// Workaround for a VTK bug involving a missing Xt App Context causing a segfault in
	// CreateRepeatingTimer.
	myRenderWindow->Render();

	// Enable timer on the render window.
	myRenderWindow->GetInteractor()->CreateRepeatingTimer(17);
	myRenderWindow->GetInteractor()->AddObserver(vtkCommand::TimerEvent, timerCallback);

	// Create callback function to switch display modes (using the '1' and '2' keys)
	auto modeSwitchFunc =
	        [](vtkObject* caller, unsigned long eventId, void* clientData, void* callData)
	        {
		        StuproApplication & client = *((StuproApplication*)clientData);

		        // Get the interactor to determine the pressed key.
		        vtkRenderWindowInteractor * interactor = (vtkRenderWindowInteractor*)caller;

		        if (interactor->GetKeyCode() == 49)// 1 key
		        {
			        client.myDisplayMode = DisplayGlobe;
		        }
		        else if (interactor->GetKeyCode() == 50) // 2 key
		        {
			        client.myDisplayMode = DisplayMap;
		        }
	        };

	// Create and assign callback for mode switch function.
	vtkSmartPointer<vtkCallbackCommand> modeSwitchCallback =
	        vtkSmartPointer<vtkCallbackCommand>::New();
	modeSwitchCallback->SetCallback(modeSwitchFunc);
	modeSwitchCallback->SetClientData(this);
	myRenderWindow->GetInteractor()->AddObserver(vtkCommand::KeyPressEvent, modeSwitchCallback);
}

void StuproApplication::initGlobe()
{
	myGlobe = makeUnique<Globe>(*myRenderer);
}
