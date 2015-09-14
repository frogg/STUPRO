#ifndef __vtkPVStuproView_h
#define __vtkPVStuproView_h

#include "vtkPVRenderView.h"

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
};

#endif
