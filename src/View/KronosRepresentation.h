#ifndef __KronosRepresentation_h
#define __KronosRepresentation_h
#include <Reader/DataReader/Data.hpp>
#include <View/KronosRepresentation/KronosLabelMapper.h>

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

#include "vtkActor2D.h"
#include "vtkLabelPlacementMapper.h"
#include "vtkPointSetToLabelHierarchy.h"
#include <qstring.h>


class VTK_EXPORT KronosRepresentation : public vtkGeometryRepresentationWithFaces
{
public:
	static KronosRepresentation* New();

	vtkTypeMacro(KronosRepresentation, vtkGeometryRepresentationWithFaces);

  void PrintSelf(ostream& os, vtkIndent indent) override;
  /**
   * Sets the visibility of the representation.
   * @param val visibility variable
   */
  void SetVisibility(bool val) override;

protected:
    //constructor and destructor
    KronosRepresentation();
    ~KronosRepresentation();

    virtual int RequestInformation(vtkInformation *request,
                           vtkInformationVector **inputVector,
                           vtkInformationVector *outputVector) override;
    virtual int RequestData(vtkInformation*,
                            vtkInformationVector**, vtkInformationVector*) override;
    int FillInputPortInformation(int, vtkInformation *info) override;
    /**
     * Called on init, addes actors of this representation to the view
     */
    virtual bool AddToView(vtkView* view) override;
    /**
     * Called on close, removes actors of this representation from the view
     */
    virtual bool RemoveFromView(vtkView* view) override;
    /**
     *  Reprentation for the Cities Data
     */
    void CitiesRepresentation(vtkPolyData *input);
    /**
     *  Reprentation for the Tweet Data
     */
    void TweetRepresentation(vtkPolyData *input);

private:
    KronosRepresentation(const KronosRepresentation&); // Not implemented
    void operator=(const KronosRepresentation&); // Not implemented
    /**
     * Display an error message and remember that this filter does not hold valid data.
     * @param message The error message to be shown to the user
     */
    void fail(QString message);
    //Mapper for the points
    vtkSmartPointer<vtkPolyDataMapper> pointMapper;
    //Mapper for the labels
    vtkSmartPointer<KronosLabelMapper> labelMapper;
    //Actor which adds the Lables to the view
    vtkSmartPointer<vtkActor2D> labelActor;
    //Filters the Data according to the priority
    vtkSmartPointer<vtkPointSetToLabelHierarchy> pointSetToLabelHierarchyFilter;
    //Input data
    vtkSmartPointer<vtkPolyData> inputData;
    //Boolean flag used to enable/disable the depthbuffer
    bool useDepthBuffer;
    //Boolean flag denoting whether there was an error.
    bool error;
    //Boolean flag denoting whether paraview is in powerwallmode
    bool inPowerwallMode;
    //Holds the current DataType
    Data::Type currentDataType;
};

#endif
