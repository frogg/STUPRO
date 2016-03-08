#include <cmath>
#include <stddef.h>
#include <Utils/Config/Configuration.hpp>
#include <Utils/Math/Interpolator.hpp>
#include <Utils/Math/GeographicFunctions.hpp>
#include <Utils/Misc/MakeUnique.hpp>
#include <pqApplicationCore.h>
#include <QCoreApplication>
#include <QEventLoop>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <KronosView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCubeSource.h>
#include <vtkProperty.h>
#include <vtkPVInteractorStyle.h>

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

void KronosView::animateMove(double latitude, double longitude, double distance) {
	this->animateMove(latitude, longitude, distance, this->getAnimationDuration());
}

void KronosView::animateMove(double latitude, double longitude, double distance,
                             double animationDuration) {
	bool onGlobe = this->getDisplayMode() == Globe::DisplayGlobe;

	// camera positions
	vtkCamera* camera = this->GetActiveCamera();
	Vector3d from;
	camera->GetPosition(from.array());
	if (onGlobe) {
		// translate the camera position into a gps location
		from = cartesianToSpherical(Cartesian<double>(from));
	}

	// on the globe, the target position already is given in gps coordinates, but on the map we have
	// to translate the target position to map coordinates
	Vector3d to(longitude, latitude, distance);
	if (!onGlobe) {
		to = sphericalToCartesianFlat(Spherical<double>(to));
	}

	// focal point positions
	Vector3d focalFrom;
	camera->GetFocalPoint(focalFrom.array());

	// on a globe, the focal point should be at the center of the globe
	// on a map, the focal point will be directly below the camera's target location
	Vector3d focalTo((onGlobe ? 0.0 : to.x), (onGlobe ? 0.0 : to.y), 0.0);

	// calculate a distance control point based on the distance of the animation for creating a
	// zoom-out-and-back-in effect
	Vector2d latLongDelta(to.x - from.x, to.y - from.y);
	double distanceControlPoint = latLongDelta.lengthTyped() / (onGlobe ? 100 : 1) / Configuration::getInstance().getFloat("globe.radius") + std::min(from.z,
	                              to.z);

	// get the turn angle of the camera, so we can animate it
	double rollFrom = camera->GetRoll();

	// animate the camera movement frame after frame for a total duration of about animationDuration
	double animationTime = 0.0;
	while (animationTime <= animationDuration) {
		// get the start timestamp of the animation to later calculate the render time
		auto start = std::chrono::steady_clock::now();

		// calculate how far we are in the animation in a range of [0, 1]
		double t = std::min(animationTime / animationDuration, 1.0);

		// interpolate the camera position
		Vector3d pos(
		    Interpolator::quadraticInOut(t, from.x, to.x),
		    Interpolator::quadraticInOut(t, from.y, to.y),
		    Interpolator::quadraticBezier(t, from.z, to.z, distanceControlPoint)
		);
		// if we're on a globe, translate the camera's gps coordinates back to cartesian coordinates
		if (onGlobe) {
			pos = sphericalToCartesian(Spherical<double>(pos));
		}

		// interpolate the focal point
		Vector3d focal = Interpolator::quadraticInOut(t, focalFrom, focalTo);

		// set the new camera position
		camera->SetPosition(pos.array());
		camera->SetFocalPoint(focal.array());

		// the up-vector of the camera should always face north (0°) at the end
		camera->SetRoll(Interpolator::quadraticInOut(t, rollFrom, 0.0));

		// render the scene with the new camera position
		this->ResetCameraClippingRange();
		this->GetRenderWindow()->Render();
		this->GetInteractorStyle()->SetCenterOfRotation(focal.array());

		// make sure paraview doesn't become completely unresponsive during the transition
		// NOTE: allowing user input events to be processed would mean allowing two animations to
		// be played at once, with each setting the camera to different locations. It'd be a mess.
		// TODO: replace with ServerUtils implementation in feature/downloaderRework when merged
		if (pqApplicationCore::instance() != nullptr) {
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}

		// calculate how long the frame took to render
		auto end = std::chrono::steady_clock::now();
		std::chrono::duration<double> delta = end - start;

		// update the total render time
		animationTime += delta.count();

		// if the current time step was not the last but the new animation time exceeds the
		// animation duration, set it to the duration to ensure that the last animation frame will
		// be rendered and the animation doesn't non-deterministically stop somewhere in between
		if (t < 1.0 && animationTime > animationDuration) {
			animationTime = animationDuration;
		}
	}
}

void KronosView::moveCamera(float latitude, float longitude) {
	this->moveCamera(latitude, longitude, this->getDefaultAnimationTargetDistance());
}

void KronosView::moveCamera(float latitude, float longitude, float distance) {
	// make the distance scale with the globe radius
	distance *= Configuration::getInstance().getFloat("globe.radius");

	// left-right, up-down, close-far
	Vector3d position(longitude, latitude, distance);

	// probably important for map view
	Vector3d focus(0.0, 0.0, 0.0);

	if (this->isAnimated()) {
		this->animateMove(latitude, longitude, distance);
	} else {
		if (this->displayMode == Globe::DisplayGlobe) {
			position = sphericalToCartesian(Spherical<double>(position));
		} else {
			position = sphericalToCartesianFlat(Spherical<double>(position));

			focus.x = position.x;
			focus.y = position.y;
		}
		vtkCamera* camera = GetActiveCamera();

		camera->SetPosition(position.array());
		camera->SetFocalPoint(focus.array());
		camera->SetRoll(0.0);

		this->ResetCameraClippingRange();
		GetRenderWindow()->Render();
	}
}

float KronosView::getCameraDistance() {
	vtkCamera* cam = GetActiveCamera();
	double x, y, z;
	cam->GetPosition(x, y, z);

	double cx, cy, cz;
	cam->GetFocalPoint(cx, cy, cz);

	x -= cx;
	y -= cy;
	z -= cz;

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

void KronosView::setAnimated(bool animated) {
	this->animated = animated;
}

bool KronosView::isAnimated() {
	return this->animated;
}

void KronosView::setAnimationDuration(double seconds) {
	this->animationDuration = seconds;
}

double KronosView::getAnimationDuration() const {
	return this->animationDuration;
}

void KronosView::setDefaultAnimationTargetDistance(double distance) {
	this->defaultAnimationTargetDistance = distance;
}

double KronosView::getDefaultAnimationTargetDistance() const {
	return this->defaultAnimationTargetDistance;
}
