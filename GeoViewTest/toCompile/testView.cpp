#include <QApplication>
#include <vtkCamera.h>
#include <vtkGeoAlignedImageRepresentation.h>
#include <vtkGeoEdgeStrategy.h>
#include <vtkGeoGlobeSource.h>
#include <vtkGeoRandomGraphSource.h>
#include <vtkGeoTerrain.h>
#include <vtkGeoView.h>
#include <vtkRandomGraphSource.h>
#include <vtkRenderedGraphRepresentation.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>

#include "GIBSImageSource.h"

#define VTK_CREATE(type,name) \
vtkSmartPointer<type> name = vtkSmartPointer<type>::New();

int TestGeoView(int argc, char* argv[])
{
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

	GIBSImageSource* gibsImageSource = GIBSImageSource::New();
	gibsImageSource->Initialize();

	vtkSmartPointer<vtkGeoAlignedImageRepresentation> imageRep =
	vtkSmartPointer<vtkGeoAlignedImageRepresentation>::New();
	imageRep->SetSource(gibsImageSource);
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

	view->Render();

	// BUG: Need to call it twice in order to show the imagery on the globe.
	view->Render();

	view->GetInteractor()->Initialize();
	view->GetInteractor()->Start();
}

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	return TestGeoView(argc, argv);
}
