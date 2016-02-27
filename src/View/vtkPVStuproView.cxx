#include <cmath>
#include <stddef.h>
#include <Utils/Config/Configuration.hpp>
#include <Utils/Math/Interpolator.hpp>
#include <Utils/Math/SphericalCoordinateFunctions.h>
#include <Utils/Misc/MakeUnique.hpp>
#include <pqApplicationCore.h>
#include <QCoreApplication>
#include <QEventLoop>
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

void vtkPVStuproView::animateMove(double latitude, double longitude, double distance)
{
	bool onGlobe = this->getDisplayMode() == Globe::DisplayGlobe;

	// camera positions
	vtkCamera* camera = this->GetActiveCamera();
	Vector3d from;
	camera->GetPosition(from.array());
	if (onGlobe) {
		// translate the camera position into a gps location
		from = cartesianToSpherical(from);
	}

	// on the globe, the target position already is given in gps coordinates, but on the map we have
	// to translate the target position to map coordinates
	Vector3d to(longitude / (onGlobe ? 1 : 90), latitude / (onGlobe ? 1 : 90), distance);

	// focal point positions
	Vector3d focalFrom;
	camera->GetFocalPoint(focalFrom.array());

	// on a globe, the focal point should be at the center of the globe
	// on a map, the focal point will be directly below the camera's target location
	Vector3d focalTo((onGlobe ? 0.0 : to.x), (onGlobe ? 0.0 : to.y), 0.0);

	// calculate a distance control point based on the distance of the animation for creating a
	// zoom-out-and-back-in effect
	Vector2d latLongDelta(to.x - from.x, to.y - from.y);
	double distanceControlPoint = latLongDelta.lengthTyped() / (onGlobe ? 100 : 1) + std::min(from.z, to.z);

	// animation
	for (int i = 0; i <= 30; i++) {
		double t = i / 30.0;

		// interpolate the camera position
		Vector3d pos(
			Interpolator::quadraticInOut(t, from.x, to.x),
			Interpolator::quadraticInOut(t, from.y, to.y),
			Interpolator::quadraticBezier(t, from.z, to.z, distanceControlPoint)
		);
		// if we're on a globe, translate the camera's gps coordinates back to cartesian coordinates
		if (onGlobe) {
			pos = sphericalToCartesian(pos);
		}

		// interpolate the focal point
		Vector3d focal = Interpolator::quadraticInOut(t, focalFrom, focalTo);

		// set the new camera position
		camera->SetPosition(pos.array());
		camera->SetFocalPoint(focal.array());

		// the up-vector of the camera should always face north
		camera->SetRoll(0.0);

		// render the scene with the new camera position
		this->ResetCameraClippingRange();
		this->GetRenderWindow()->Render();

		// make sure paraview doesn't become completely unresponsive during the transition
		// NOTE: allowing user input events to be processed would mean allowing two animations to
		// be played at once, with each setting the camera to different locations. It'd be a mess.
		// TODO: replace with ServerUtils implementation in feature/downloaderRework when merged
		if (pqApplicationCore::instance() != nullptr) {
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}
	}
}

void vtkPVStuproView::moveCamera(float latitude, float longitude)
{
	this->moveCamera(latitude, longitude, 0.3);
}

void vtkPVStuproView::moveCamera(float latitude, float longitude, float distance)
{
	// left-right, up-down, close-far
	Vector3d position(0.0, 0.0, 2.6);

	// probably important for map view
	Vector3d focus(0.0, 0.0, 0.0);

	if (this->isAnimated()) {
		this->animateMove(latitude, longitude, distance);
	} else {
		if (this->displayMode == Globe::DisplayGlobe) {
			position = sphericalToCartesian(Vector3d(longitude, latitude, distance));
		} else {
			position.x = longitude / 90;
			position.y = latitude / 180;
			position.z = distance;

			focus.x = position.x;
			focus.y = position.y;
			focus.z = 0;
		}
		vtkCamera* camera = GetActiveCamera();

		camera->SetPosition(position.array());
		camera->SetFocalPoint(focus.array());
		camera->SetRoll(0.0);

		this->ResetCameraClippingRange();
		GetRenderWindow()->Render();
	}
}

float vtkPVStuproView::getCameraDistance()
{
	vtkCamera* cam = GetActiveCamera();
	double x, y, z;
	cam->GetPosition(x, y, z);

	double cx, cy, cz;
	cam->GetFocalPoint(cx, cy, cz);

	x -= cx;
	y -= cy;
	z -= cz;

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

void vtkPVStuproView::setAnimated(bool animated)
{
	this->animated = animated;
}

bool vtkPVStuproView::isAnimated()
{
	return this->animated;
}
