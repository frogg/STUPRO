#ifndef __vtkPVStuproView_h
#define __vtkPVStuproView_h

#include "vtkPVRenderView.h"
#include "vtkSmartPointer.h"
#include "vtkTexture.h"

class VTK_EXPORT vtkPVStuproView : public vtkPVRenderView
{
public:
  static vtkPVStuproView* New();
  vtkTypeMacro(vtkPVStuproView, vtkPVRenderView);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void Initialize(unsigned int id);

  std::string readFile(std::string filename);

  vtkSmartPointer<vtkTexture> getTextureForImageName(std::string picture, std::string heightPicture);

protected:
  vtkPVStuproView();
  ~vtkPVStuproView();

private:
  vtkPVStuproView(const vtkPVStuproView&); // Not implemented
  void operator=(const vtkPVStuproView&); // Not implemented
};

#endif
