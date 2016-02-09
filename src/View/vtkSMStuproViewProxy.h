#ifndef KRONOS_VIEW_VTK_SM_STUPRO_VIEW_PROXY_H
#define KRONOS_VIEW_VTK_SM_STUPRO_VIEW_PROXY_H

#include "vtkSMRenderViewProxy.h"

class vtkSMRenderViewProxy;

class VTK_EXPORT vtkSMStuproViewProxy : public vtkSMRenderViewProxy
{
public:
    static vtkSMStuproViewProxy* New();
    vtkTypeMacro(vtkSMStuproViewProxy, vtkSMRenderViewProxy);

    void moveCamera(float latitude, float longitude);
    void moveCamera(float latitude, float longitude, float distance);

protected:
    vtkSMStuproViewProxy();
    ~vtkSMStuproViewProxy();

private:
    vtkSMStuproViewProxy(const vtkSMStuproViewProxy&); // not implemented
    void operator=(const vtkSMStuproViewProxy&); // not implemented
};

#endif
