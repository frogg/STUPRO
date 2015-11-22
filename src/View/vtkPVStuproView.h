#ifndef KRONOS_PVVIEW_H
#define KRONOS_PVVIEW_H

#include <vtkPVRenderView.h>
#include <vtkSetGet.h>
#include <vtkWin32Header.h>
#include <iostream>
#include <memory>

// TODO/FIXME: Somehow, VS13 needs an include for std::memory here instead of
// a normal forward-declarion.
#include <Globe/Globe.hpp>
//class Globe;

/**
 * This class represents our own, custom vtkPVRenderView. It is responsible
 * for initializing all the relevant variables and objects used later on.
 */
class VTK_EXPORT vtkPVStuproView : public vtkPVRenderView
{
public:
	/**
	 * Enum giving us all available, different display modes for the
	 * globe.
	 */
	enum DisplayMode
	{
		DisplayGlobe, DisplayMap
	};
	
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
	 * Upon call, switches the currently used display mode to the opposite,
	 * basically by inverting this->displayMode.
	 */
	void switchCurrentDisplayMode();

	// TODO: Make a global config file.
	float getGlobeRadius() const;
	
	/**
	 * Returns the current display mode.
	 *
	 * @return the current display mode
	 */
	DisplayMode getDisplayMode() const;

protected:
	/**
	 * Constructor and destructor are protected, so these never get
	 * called outside of this class. Therefore it can only be
	 * instantiated by a vtkSmartPointer.
	 */
	vtkPVStuproView()
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
	 * FIXME: Doesn't work in PV as of 18-11-2015.
	 */
	void registerTimerCallback();

	/**
	 * Initializes an instance of the this->globe as a unique pointer.
	 */
	void initGlobe();


	DisplayMode displayMode;
	std::unique_ptr<Globe> globe;
	float globeRadius;
	float planeSize;
	float displayModeInterpolation;
	float heightFactor;
};

#endif
