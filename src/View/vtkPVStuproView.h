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
class VTK_EXPORT vtkPVStuproView : public vtkPVRenderView
{
public:

	/**
	 * A static new-method needed for any vtk-related initialization
	 * used by vtkSmartPointers. Gonna be implemented in the .cxx
	 * using the vtkStandardNewMacro.
	 *
	 * @return a new instance of this view via a pointer
	 */
	static vtkPVStuproView* New();

	/**
	 * Needed for some vtk-inheritance-stuff.
	 */
	vtkTypeMacro(vtkPVStuproView, vtkPVRenderView)

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
	 * @param distance  the distance from the globe's or map's surface to move the camera to
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

	Globe * getGlobe() const;

protected:
	/**
	 * Constructor and destructor are protected, so these never get
	 * called outside of this class. Therefore it can only be
	 * instantiated by a vtkSmartPointer.
	 */
	vtkPVStuproView() :
		displayMode(Globe::DisplayGlobe)
	{
	};

	~vtkPVStuproView()
	{
	};

private:
	/**
	 * This method is not implemented due to vtks architecture.
	 * This will remove the copy-constructor.
	 */
	vtkPVStuproView(const vtkPVStuproView&);

	/**
	* This method is not implemented due to vtks architecture.
	* This will remove the =-assignment (and copying).
	*/
	void operator=(const vtkPVStuproView&);

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

	Globe::DisplayMode displayMode;
	std::unique_ptr<Globe> globe;
	vtkSmartPointer<vtkCallbackCommand> cameraModifiedCallback;
	bool animated = true;
};

#endif
