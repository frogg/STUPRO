
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
#include <vtkViewTheme.h>

#define VTK_CREATE(type,name) \
vtkSmartPointer<type> name = vtkSmartPointer<type>::New();

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
    
    vtkViewTheme* theme = vtkViewTheme::New();
    view->ApplyViewTheme(theme);
    theme->Delete();
    
    view->Render();
    
    // BUG: Need to call it twice in order to show the imagery on the globe.
    view->Render();
    
    view->GetInteractor()->Initialize();
    view->GetInteractor()->Start();
}

int main(int argc, char* argv[])
{
    return TestGeoView(argc, argv);
}
