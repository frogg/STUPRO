#ifndef __vtkGlobeSource_h
#define __vtkGlobeSource_h

#include "vtkSphereSource.h"

class vtkGlobeSource : public vtkSphereSource {
public:
	vtkTypeMacro(vtkGlobeSource, vtkPolyDataAlgorithm);

	// Description:
	// Construct sphere with radius=0.5 and default resolution 8 in both Phi
	// and Theta directions. Theta ranges from (0,360) and phi (0,180) degrees.
	static vtkGlobeSource *New();

protected:
	vtkGlobeSource(int res = 8);
	~vtkGlobeSource() {}

private:
	vtkGlobeSource(const vtkGlobeSource&);  // Not implemented.
	void operator=(const vtkGlobeSource&);  // Not implemented.
};

#endif