#ifndef __vtkPVStuproView_h
#define __vtkPVStuproView_h

#include <vtkPVRenderView.h>
#include <vtkSetGet.h>
#include <vtkWin32Header.h>
#include <iostream>
#include <memory>

class Globe;

class VTK_EXPORT vtkPVStuproView : public vtkPVRenderView
{
public:
	enum DisplayMode
	{
		DisplayGlobe, DisplayMap
	};
	
	static vtkPVStuproView* New();
	vtkTypeMacro(vtkPVStuproView, vtkPVRenderView);
	
	void PrintSelf(ostream& os, vtkIndent indent);

	virtual void Initialize(unsigned int id);

	void switchCurrentDisplayMode();

	float getGlobeRadius();
	DisplayMode getDisplayMode();

protected:
	vtkPVStuproView();
	~vtkPVStuproView();

private:
	vtkPVStuproView(const vtkPVStuproView&); // Not implemented
	void operator=(const vtkPVStuproView&); // Not implemented

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
