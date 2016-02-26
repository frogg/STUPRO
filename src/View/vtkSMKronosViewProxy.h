#ifndef KRONOS_VIEW_VTK_SM_STUPRO_VIEW_PROXY_H
#define KRONOS_VIEW_VTK_SM_STUPRO_VIEW_PROXY_H

#include "vtkSMRenderViewProxy.h"

class vtkSMRenderViewProxy;

class VTK_EXPORT vtkSMKronosViewProxy : public vtkSMRenderViewProxy {
public:
	static vtkSMKronosViewProxy* New();
	vtkTypeMacro(vtkSMKronosViewProxy, vtkSMRenderViewProxy);

	void moveCamera(float latitude, float longitude);
	void moveCamera(float latitude, float longitude, float distance);

protected:
	vtkSMKronosViewProxy();
	~vtkSMKronosViewProxy();

private:
	vtkSMKronosViewProxy(const vtkSMKronosViewProxy&); // not implemented
	void operator=(const vtkSMKronosViewProxy&); // not implemented
};

#endif
