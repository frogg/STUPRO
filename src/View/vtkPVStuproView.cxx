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
	initGlobe();
}

void vtkPVStuproView::initParameters()
{
	// Initialize parameters.
	this->displayMode = Globe::DisplayGlobe;
}

void vtkPVStuproView::initRenderer()
{
	this->cameraModifiedCallback = vtkCallbackCommand::New();

    this->cameraModifiedCallback->SetCallback(
		[](vtkObject* object, unsigned long eid, void* clientdata, void *calldata)
		{
			vtkPVStuproView * view = (vtkPVStuproView *)clientdata;
			if (view->getGlobe()) {
				view->getGlobe()->onCameraChanged();
			}
		});

    this->cameraModifiedCallback->SetClientData(this);

	this->GetRenderer()->AddObserver(vtkCommand::ResetCameraClippingRangeEvent, this->cameraModifiedCallback);
}

void vtkPVStuproView::initGlobe()
{
	const Configuration & config = Configuration::getInstance();

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
	this->displayMode = this->displayMode == Globe::DisplayGlobe ? Globe::DisplayMap : Globe::DisplayGlobe;
	this->globe->setDisplayMode(this->displayMode);

	// Render the view again.
	// GetRenderWindow()->Render();
}

Globe::DisplayMode vtkPVStuproView::getDisplayMode() const
{
	return this->displayMode;
}

Globe * vtkPVStuproView::getGlobe() const
{
	return this->globe.get();
}
