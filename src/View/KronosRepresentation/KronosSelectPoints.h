/*=========================================================================

  This class is based on the vtkSelectVisiblePoints class.
  with a fix that labels are drawn in 2D mode too.

=========================================================================*/

#ifndef __KronosSelectPoints_h
#define __KronosSelectPoints_h

#include "vtkRenderingCoreModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class vtkRenderer;
class vtkMatrix4x4;

class KronosSelectPoints : public vtkPolyDataAlgorithm {
public:
	vtkTypeMacro(KronosSelectPoints, vtkPolyDataAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Instantiate object with no renderer; window selection turned off;
	// tolerance set to 0.01; and select invisible off.
	static KronosSelectPoints* New();

	// Description:
	// Specify the renderer in which the visibility computation is to be
	// performed.
	void SetRenderer(vtkRenderer* ren) {
		if (this->Renderer != ren) {
			this->Renderer = ren;
			this->Modified();
		}
	}
	vtkRenderer* GetRenderer() {
		return this->Renderer;
	}

	// Description:
	// Set/Get the flag which enables selection in a rectangular display
	// region.
	vtkSetMacro(SelectionWindow, int);
	vtkGetMacro(SelectionWindow, int);
	vtkBooleanMacro(SelectionWindow, int);

	// Description:
	// Specify the selection window in display coordinates. You must specify
	// a rectangular region using (xmin,xmax,ymin,ymax).
	vtkSetVector4Macro(Selection, int);
	vtkGetVectorMacro(Selection, int, 4);

	// Description:
	// Set/Get the flag which enables inverse selection; i.e., invisible points
	// are selected.
	vtkSetMacro(SelectInvisible, int);
	vtkGetMacro(SelectInvisible, int);
	vtkBooleanMacro(SelectInvisible, int);

	// Description:
	// Set/Get a tolerance to use to determine whether a point is visible. A
	// tolerance is usually required because the conversion from world space
	// to display space during rendering introduces numerical round-off.
	vtkSetClampMacro(Tolerance, double, 0.0, VTK_DOUBLE_MAX);
	vtkGetMacro(Tolerance, double);

	// Description:
	// Requires the renderer to be set. Populates the composite perspective transform
	// and returns a pointer to the Z-buffer (that must be deleted) if getZbuff is set.
	float* Initialize(bool getZbuff);

	// Description:
	// Tests if a point x is being occluded or not against the Z-Buffer array passed in by
	// zPtr. Call Initialize before calling this method.
	bool IsPointOccluded(const double x[3], const float* zPtr);

	// Description:
	// Return MTime also considering the renderer.
	unsigned long GetMTime();

protected:
	KronosSelectPoints();
	~KronosSelectPoints();

	virtual int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*);
	virtual int FillInputPortInformation(int port, vtkInformation* info);

	vtkRenderer* Renderer;
	vtkMatrix4x4* CompositePerspectiveTransform;

	int SelectionWindow;
	int Selection[4];
	int InternalSelection[4];
	int SelectInvisible;
	double Tolerance;

private:
	KronosSelectPoints(const KronosSelectPoints&);  // Not implemented.
	void operator=(const KronosSelectPoints&);  // Not implemented.
};

#endif
