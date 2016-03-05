#ifndef KRONOS_PVVIEW_H
#define KRONOS_PVVIEW_H

#include <vtkCallbackCommand.h>
#include <vtkPVRenderView.h>
#include <vtkSetGet.h>
#include <vtkSmartPointer.h>
#include <vtkWin32Header.h>
#include <memory>
#include <Globe/Globe.hpp>

/**
 * This class represents our own, custom vtkPVRenderView. It is responsible
 * for initializing all the relevant variables and objects used later on.
 */
class VTK_EXPORT KronosView : public vtkPVRenderView {
public:

	/**
	 * A static new-method needed for any vtk-related initialization
	 * used by vtkSmartPointers. Gonna be implemented in the .cxx
	 * using the vtkStandardNewMacro.
	 *
	 * @return a new instance of this view via a pointer
	 */
	static KronosView* New();

	/**
	 * Needed for some vtk-inheritance-stuff.
	 */
	vtkTypeMacro(KronosView, vtkPVRenderView)

	/**
	 * This method gets called any time the view is going
	 * to be initialized. Basically, this is the constructor of
	 * the view.
	 *
	 * @param id the id of the view, gets set by the calling instance, used for Superclass::Initialize(id)
	 */
	virtual void Initialize(unsigned int id) override;

	/**
	 * Moves the camera the specified latitude and longitude, keeping the distance the same as it was before and
	 * pointing the camera directly at the globe's center.
	 *
	 * @param latitude  the latitude to move the camera to
	 * @param longitude the longitude to move the camera to
	 */
	void moveCamera(float latitude, float longitude);

	/**
	 * Moves the camera the specified latitude and longitude, altering its distance and pointing the camera directly at
	 * the globe's center.
	 *
	 * @param latitude  the latitude to move the camera to
	 * @param longitude the longitude to move the camera to
	 * @param distance  the distance from the globe's or map's surface in multiples of the globe
	 *                  radius to move the camera to
	 */
	void moveCamera(float latitude, float longitude, float distance);

	/**
	 * Returns the distance between the camera and the globe's center point.
	 *
	 * This method should be const, but cannot be set to const as VTK is not const-correct.
	 */
	float getCameraDistance();

	/**
	 * Upon call, switches the currently used display mode to the opposite,
	 * basically by inverting this->displayMode.
	 */
	void switchCurrentDisplayMode();

	/**
	 * Returns the current display mode.
	 *
	 * @return the current display mode
	 */
	Globe::DisplayMode getDisplayMode() const;

	/**
	 * @param animated if the camera movement should be animated.
	 */
	void setAnimated(bool animated);

	/**
	 * @returns if the camera movement is animated.
	 */
	bool isAnimated();

	/**
	 * @param seconds the camera animation duration in seconds
	 */
	void setAnimationDuration(double seconds);

	/**
	 * @returns the camera animation duration in seconds
	 */
	double getAnimationDuration() const;

	/**
	 * @param distance the default distance from the map or globe the camera should fly to when
	 *                 animating
	 */
	void setDefaultAnimationTargetDistance(double distance);

	/**
	 * @returns the default distance from the map or globe the camera should fly to when animating
	 */
	double getDefaultAnimationTargetDistance() const;

	Globe* getGlobe() const;

protected:
	/**
	 * Constructor and destructor are protected, so these never get
	 * called outside of this class. Therefore it can only be
	 * instantiated by a vtkSmartPointer.
	 */
	KronosView() :
		displayMode(Globe::DisplayGlobe) {
	};

	~KronosView() {
	};

private:
	/**
	 * This method is not implemented due to vtks architecture.
	 * This will remove the copy-constructor.
	 */
	KronosView(const KronosView&);

	/**
	* This method is not implemented due to vtks architecture.
	* This will remove the =-assignment (and copying).
	*/
	void operator=(const KronosView&);

	/**
	 * TODO: Maybe remove this method using the global config?
	 */
	void initParameters();

	/**
	 * Initializes all render-relevant objects, i.e. the interactor
	 * and its style.
	 */
	void initRenderer();

	/**
	 * Registers all callback functions used later during runtime.
	 */
	void registerTimerCallback();

	/**
	 * Initializes an instance of the this->globe as a unique pointer.
	 */
	void initGlobe();

	/**
	 * Creates a 'smooth' transition between the current location of the camera and the given one.
	 *
	 * @param latitude  the latitude to move the camera to
	 * @param longitude the longitude to move the camera to
	 * @param distance  the distance from the globe's or map's surface to move the camera to
	 */
	void animateMove(double latitude, double longitude, double distance);

	/**
	 * Creates a 'smooth' transition between the current location of the camera and the given one.
	 *
	 * @param latitude          the latitude to move the camera to
	 * @param longitude         the longitude to move the camera to
	 * @param distance          the distance from the globe's or map's surface to move the camera to
	 * @param animationDuration the time in seconds the animation should take
	 */
	void animateMove(double latitude, double longitude, double distance, double animationDuration);

	Globe::DisplayMode displayMode;
	std::unique_ptr<Globe> globe;
	vtkSmartPointer<vtkCallbackCommand> cameraModifiedCallback;
	vtkSmartPointer<vtkCallbackCommand> activeCameraCallback;

	/** determines whether camera movement should be animated or not */
	bool animated = true;
	/** the duration that should be used when animateMove is called */
	double animationDuration = 2.0;
	/**
	 * the default distance from the globe or map the camera should fly to when animateMove is
	 * called without a distance
	 */
	double defaultAnimationTargetDistance = 0.3;
};

#endif
