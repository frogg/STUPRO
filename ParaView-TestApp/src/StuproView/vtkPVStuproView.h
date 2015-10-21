#ifndef __vtkPVStuproView_h
#define __vtkPVStuproView_h

#include "vtkPVRenderView.h"
#include "vtkSmartPointer.h"

#include "vtkOpenGLTexture.h"
#include "vtkActor.h"
#include "vtkShader2.h"

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

  std::string readFile(std::string filename) const;
  vtkSmartPointer<vtkOpenGLTexture> loadAlphaTexture(std::string rgbFile, std::string alphaFile) const;

  void initParameters();
  void initGlobe();
  void initRenderer();
  void initShaders();
  void initCallbacks();

  vtkSmartPointer<vtkActor> myPlaneActor;

  vtkSmartPointer<vtkShader2> myVertexShader;
  vtkSmartPointer<vtkShader2> myFragmentShader;

  DisplayMode myDisplayMode;

  float myGlobeRadius;
  float myPlaneSize;
  float myDisplayModeInterpolation;
  float myHeightFactor;
};

#endif
