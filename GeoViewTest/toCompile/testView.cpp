#include <vtkGeoCamera.h>
#include <vtkCamera.h>
#include <vtkGeoAlignedImageRepresentation.h>
#include <vtkGeoAlignedImageSource.h>
#include <vtkGeoEdgeStrategy.h>
#include <vtkGeoGlobeSource.h>
#include <vtkGeoRandomGraphSource.h>
#include <vtkGeoTerrain.h>
#include <vtkGeoView.h>
#include <vtkJPEGReader.h>
#include <vtkRandomGraphSource.h>
#include <vtkRenderedGraphRepresentation.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkStdString.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkObjectFactory.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkTransformTextureCoords.h>
#include <vtkTexture.h>
#include <vtkTextureMapToSphere.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader.h>
#include <vtkGeoInteractorStyle.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkCommand.h>
#include <vtkCallbackCommand.h>
#include "vtkCompassWidget.h"

#define VTK_CREATE(type,name) \
vtkSmartPointer<type> name = vtkSmartPointer<type>::New();

class interactor : public vtkGeoInteractorStyle
{
public:
    static interactor* New();
    vtkTypeMacro(interactor, vtkGeoInteractorStyle);
    int x=0;
    int y=0;
    bool leftMouseButtonDown;
    virtual void OnMouseMove() override
    {
        if(!leftMouseButtonDown){
            return;
        }
        this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                                this->Interactor->GetEventPosition()[1]);
        if (this->CurrentRenderer == NULL)
        {
            return;
        }
        double deltaX=this->Interactor->GetEventPosition()[0]-x;
        double deltaY=this->Interactor->GetEventPosition()[1]-y;
        deltaX=deltaX*pow(  1-1/sqrt(this->GeoCamera->GetDistance()/100000)   ,4);
        deltaY=deltaY*pow(  1-1/sqrt(this->GeoCamera->GetDistance()/100000)   ,4);
        x=this->Interactor->GetEventPosition()[0];
        y=this->Interactor->GetEventPosition()[1];
        
        std::cout << "x: " << x;
        
        std::cout << " deltaX: " << deltaX;
        std::cout << " deltaY: " << deltaY;
        
        std::cout << " y: " << y ;
        std::cout << " Distance: " << this->GeoCamera->GetDistance() ;
        std::cout << std::endl;
        this->GrabFocus(this->EventCallbackCommand);
        
        this->GeoCamera->SetLatitude(this->GeoCamera->GetLatitude()+deltaY*-1);
        this->GeoCamera->SetLongitude(this->GeoCamera->GetLongitude()+deltaX*-1);
        this->UpdateLights();
        this->ResetCameraClippingRange();
        this->Interactor->Render();
        this->ReleaseFocus();
        }
        //-----------------------------------------------------------------------------
        virtual void OnLeftButtonDown() override
        {
            this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                                    this->Interactor->GetEventPosition()[1]);
            if (this->CurrentRenderer == NULL)
            {
                return;
            }
            x=this->Interactor->GetEventPosition()[0];
            y=this->Interactor->GetEventPosition()[1];
            leftMouseButtonDown=true;
            
        }
        
        //-----------------------------------------------------------------------------
        virtual void OnLeftButtonUp() override
        {
            
            leftMouseButtonDown=false;
            if (!this->Interactor)
            {
                return;
            }
        }
        
};
        vtkStandardNewMacro(interactor);



int TestGeoView(int argc, char* argv[])
{
	vtkStdString image = "./earth.jpg";

	// Create the geo view.
	VTK_CREATE(vtkGeoView, view);
	view->DisplayHoverTextOff();
	view->GetRenderWindow()->SetMultiSamples(0);
	view->GetRenderWindow()->SetSize(400,400);

	vtkSmartPointer<vtkGeoTerrain> terrain =
	vtkSmartPointer<vtkGeoTerrain>::New();
	vtkSmartPointer<vtkGeoSource> terrainSource;
	vtkGeoGlobeSource* globeSource = vtkGeoGlobeSource::New();
	terrainSource.TakeReference(globeSource);
	terrainSource->Initialize();
	terrain->SetSource(terrainSource);
	view->SetTerrain(terrain);

	vtkSmartPointer<vtkGeoAlignedImageRepresentation> imageRep =
	vtkSmartPointer<vtkGeoAlignedImageRepresentation>::New();
	vtkSmartPointer<vtkGeoSource> imageSource;
	vtkGeoAlignedImageSource* alignedSource = vtkGeoAlignedImageSource::New();
	vtkSmartPointer<vtkJPEGReader> reader =
	vtkSmartPointer<vtkJPEGReader>::New();
	reader->SetFileName(image);
	reader->Update();
	alignedSource->SetImage(reader->GetOutput());
	imageSource.TakeReference(alignedSource);
	imageSource->Initialize();
	imageRep->SetSource(imageSource);
	view->AddRepresentation(imageRep);

	view->ResetCamera();
	view->GetRenderer()->GetActiveCamera()->Zoom(1.2);


    // Add an interactor
    
    vtkSmartPointer<interactor> style =
    vtkSmartPointer<interactor>::New();
    style->SetInteractor(view->GetInteractor());
    style->SetCurrentRenderer(view->GetRenderer());
    view->SetInteractorStyle(style);
    
    
	// Add a graph representation
	vtkSmartPointer<vtkGeoRandomGraphSource> graphSource =
	vtkSmartPointer<vtkGeoRandomGraphSource>::New();
	graphSource->SetNumberOfVertices(100);
	graphSource->StartWithTreeOn();
	graphSource->SetNumberOfEdges(0);
	vtkSmartPointer<vtkRenderedGraphRepresentation> graphRep =
	vtkSmartPointer<vtkRenderedGraphRepresentation>::New();
	graphRep->SetInputConnection(graphSource->GetOutputPort());
	graphRep->SetLayoutStrategyToAssignCoordinates("longitude", "latitude");
	VTK_CREATE(vtkGeoEdgeStrategy, edgeStrategy);
	graphRep->SetEdgeLayoutStrategy(edgeStrategy);
	view->AddRepresentation(graphRep);
	view->Render();

	// BUG: Need to call it twice in order to show the imagery on the globe.
	view->Render();
    view->GetInteractor()->Initialize();
    view->GetInteractor()->Start();

   
    return 0;
}

int main(int argc, char* argv[])
{
	return TestGeoView(argc, argv);
}
