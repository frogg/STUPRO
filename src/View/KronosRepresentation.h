#ifndef __KronosRepresentation_h
#define __KronosRepresentation_h

#include "vtkGeometryRepresentationWithFaces.h"
#include "vtkPVClientServerCoreRenderingModule.h" //needed for exports
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkGeometryRepresentation.h"
#include "vtkLabelPlacementMapper.h"
#include "vtkSmartPointer.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkPointSource.h"
#include "vtkLabelPlacementMapper.h"
#include "vtkActor2D.h"
#include "vtkLabelPlacementMapper.h"
#include "vtkPointSetToLabelHierarchy.h"
class VTK_EXPORT KronosRepresentation : public vtkGeometryRepresentationWithFaces
{
public:
  static KronosRepresentation* New();

  vtkTypeMacro(KronosRepresentation, vtkGeometryRepresentationWithFaces);

  void PrintSelf(ostream& os, vtkIndent indent) override;
  void SetVisibility(bool val) override;

protected:
    KronosRepresentation();
    ~KronosRepresentation();
    
    virtual int RequestData(vtkInformation*,
                            vtkInformationVector**, vtkInformationVector*) override;
    //Called when added to views.
    virtual bool AddToView(vtkView* view) override;
    virtual bool RemoveFromView(vtkView* view) override;
    int FillInputPortInformation(int, vtkInformation *info) override;

private:
    KronosRepresentation(const KronosRepresentation&); // Not implemented
    void operator=(const KronosRepresentation&); // Not implemented
    //mapper
    vtkSmartPointer<vtkPolyDataMapper> pointMapper;
    vtkSmartPointer<vtkLabelPlacementMapper> labelMapper;
    //actors
    vtkSmartPointer<vtkActor> pointActor;
    vtkSmartPointer<vtkActor2D> labelActor;
    //filter
    vtkSmartPointer<vtkPointSetToLabelHierarchy> pointSetToLabelHierarchyFilter;
};

#endif
