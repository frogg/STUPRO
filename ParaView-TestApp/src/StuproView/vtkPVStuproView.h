#ifndef __vtkPVStuproView_h
#define __vtkPVStuproView_h

#include "vtkPVRenderView.h"
#include "vtkSmartPointer.h"

#include "Globe.hpp"

#include <memory>

class VTK_EXPORT vtkPVStuproView : public vtkPVRenderView
{
public:
	static vtkPVStuproView* New();
	vtkTypeMacro(vtkPVStuproView, vtkPVRenderView);
	void PrintSelf(ostream& os, vtkIndent indent);

	virtual void Initialize(unsigned int id);

protected:
	vtkPVStuproView();
	~vtkPVStuproView();

private:
	vtkPVStuproView(const vtkPVStuproView&); // Not implemented
	void operator=(const vtkPVStuproView&); // Not implemented

	enum DisplayMode
	{
		DisplayGlobe, DisplayMap
	};

	void initParameters();
	void initGlobe();
	void initRenderer();
	void initCallbacks();

	DisplayMode myDisplayMode;

	std::unique_ptr<Globe> myGlobe;

	float myGlobeRadius;
	float myPlaneSize;
	float myDisplayModeInterpolation;
	float myHeightFactor;
};

#endif
