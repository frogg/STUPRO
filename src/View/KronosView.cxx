#include <stddef.h>
#include <Utils/Config/Configuration.hpp>
#include <Utils/Misc/MakeUnique.hpp>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <KronosView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCubeSource.h>
#include <vtkProperty.h>

vtkStandardNewMacro(KronosView);

void KronosView::Initialize(unsigned int id) {
	this->Superclass::Initialize(id);

	initParameters();
	initRenderer();
	initGlobe();
}

void KronosView::initParameters() {
	// Initialize parameters.
	this->displayMode = Globe::DisplayGlobe;
}

void KronosView::initRenderer() {
	// Check if renderer exists.
	if (this->GetRenderer()) {
		// Create camera modification callback.
		this->cameraModifiedCallback = vtkCallbackCommand::New();

		// Update globe whenever camera is changed.
		this->cameraModifiedCallback->SetCallback(
		[](vtkObject * object, unsigned long eid, void* clientdata, void* calldata) {
			KronosView* view = (KronosView*)clientdata;
			if (view->getGlobe()) {
				view->getGlobe()->onCameraChanged();
			}
		});

		// Pass pointer to this object.
		this->cameraModifiedCallback->SetClientData(this);

		// Observe all camera state changes.
		this->GetRenderer()->AddObserver(vtkCommand::ResetCameraClippingRangeEvent,
		                                 this->cameraModifiedCallback);

		// Create active camera callback.
		this->activeCameraCallback = vtkCallbackCommand::New();

		// Set camera position correctly whenever active camera is changed.
		this->activeCameraCallback->SetCallback(
		[](vtkObject * object, unsigned long eid, void* clientdata, void* calldata) {
			KronosView* view = (KronosView*)clientdata;

			// Calculate default camera position.
			float radius = Configuration::getInstance().getFloat("globe.radius");
			float outermostZoomDistance = Configuration::getInstance().getFloat("globe.zoom.farDistance");
			float distance = radius * outermostZoomDistance * 1.05f;

			// Move camera to default position.
			view->GetRenderer()->GetActiveCamera()->SetPosition(0, 0, distance);
		});

		// Pass pointer to this object.
		this->activeCameraCallback->SetClientData(this);

		// Observe all active camera changes.
		this->GetRenderer()->AddObserver(vtkCommand::ActiveCameraEvent, this->activeCameraCallback);
	}
}

void KronosView::initGlobe() {
	// Initialize a unique pointer with a new instance of the Globe
	// using the current renderer.
	this->globe = makeUnique<Globe>(*this->GetRenderer());

	// Check if renderer exists.
	if (this->GetRenderer()) {
		// Create an invisible box around the globe to prevent incorrect clipping range.
		float radius = Configuration::getInstance().getFloat("globe.radius");

		vtkSmartPointer<vtkCubeSource> cube = vtkCubeSource::New();
		cube->SetXLength(radius * 2);
		cube->SetYLength(radius * 2);
		cube->SetZLength(radius * 2);

		vtkSmartPointer<vtkPolyDataMapper> cubeMapper = vtkPolyDataMapper::New();
		cubeMapper->SetInputConnection(cube->GetOutputPort());

		vtkSmartPointer<vtkActor> cubeActor = vtkActor::New();
		cubeActor->SetMapper(cubeMapper);
		cubeActor->GetProperty()->SetOpacity(0);

		this->GetRenderer()->AddActor(cubeActor);
	}
}

void KronosView::moveCamera(float latitude, float longitude) {
	this->moveCamera(latitude, longitude, this->getCameraDistance());
}

void KronosView::moveCamera(float latitude, float longitude, float distance) {
	// left-right, up-down, close-far
	Vector3d position(0.0, 0.0, 2.6);

	// probably important for map view
	Vector3d focus(0.0, 0.0, 0.0);

	if (this->displayMode == Globe::DisplayGlobe) {
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

float KronosView::getCameraDistance() {
	vtkCamera* cam = GetActiveCamera();
	double x, y, z;
	cam->GetPosition(x, y, z);
	return sqrt(x * x + y * y + z * z);
}

void KronosView::switchCurrentDisplayMode() {
	// Invert the display mode and set the interpolation using a static cast.
	this->displayMode = this->displayMode == Globe::DisplayGlobe ? Globe::DisplayMap :
	                    Globe::DisplayGlobe;
	this->globe->setDisplayMode(this->displayMode);

	// Render the view again.
	// GetRenderWindow()->Render();
}

Globe::DisplayMode KronosView::getDisplayMode() const {
	return this->displayMode;
}

Globe* KronosView::getGlobe() const {
	return this->globe.get();
}
