#include "vtkRenderView.h"
#include "vtkRenderWindow.h"
/*
int main(int argc, char* argv[]){
	VTK_CREATE(vtkRenderView, view);
	view -> GetRenderWindow() -> SetSize(400,400);
	view -> Render();
	return 0;
}
*/

#include "vtkBMPReader.h"
#include "vtkCamera.h"
#include "vtkGeoAlignedImageRepresentation.h"
#include "vtkGeoAlignedImageSource.h"
#include "vtkGeoEdgeStrategy.h"
#include "vtkGeoFileImageSource.h"
#include "vtkGeoFileTerrainSource.h"
#include "vtkGeoGlobeSource.h"
#include "vtkGeoProjection.h"
#include "vtkGeoProjectionSource.h"
#include "vtkGeoRandomGraphSource.h"
#include "vtkGeoSphereTransform.h"
#include "vtkGeoTerrain.h"
#include "vtkGeoTerrainNode.h"
#include "vtkGeoTerrain2D.h"
#include "vtkGeoTransform.h"
#include "vtkGeoView.h"
#include "vtkGeoView2D.h"
#include "vtkGraphLayoutView.h"
#include "vtkJPEGReader.h"
#include "vtkRenderedGraphRepresentation.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkStdString.h"
#include "vtkTestUtilities.h"
#include "vtkTIFFReader.h"
#include "vtkViewTheme.h"
#include "vtkViewUpdater.h"

#include <vtksys/SystemTools.hxx>

#include "LLGeoSource.h"

#define VTK_CREATE(type,name) \
vtkSmartPointer<type> name = vtkSmartPointer<type>::New();

int TestGeoView(int argc, char* argv[])
{
    //char* image2 = vtkTestUtilities::ExpandDataFileName(argc, argv, "quappi2.jpg");
    //char* image = vtkTestUtilities::ExpandDataFileName(argc, argv, "quappi.jpg");
    const char* image2 = "/Users/larissa/Desktop/test2/VTKData/quappi2.jpg";
    const char* image = "/Users/larissa/Desktop/test2/VTKData/weltkarte.jpg";
    //char* image = vtkTestUtilities::ExpandDataFileName("","","VTKData/quappi.jpg");
    vtkStdString imageReadPath = ".";
    vtkStdString imageSavePath = ".";
    vtkStdString imageFile = image;
    vtkStdString terrainReadPath = ".";
    vtkStdString terrainSavePath = ".";
    for (int i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-I"))
        {
            continue;
        }
        if (!strcmp(argv[i], "-D") ||
            !strcmp(argv[i], "-T") ||
            !strcmp(argv[i], "-V"))
        {
            ++i;
            continue;
        }
        if (!strcmp(argv[i], "-IS"))
        {
            imageSavePath = argv[++i];
        }
        else if (!strcmp(argv[i], "-TS"))
        {
            terrainSavePath = argv[++i];
        }
        else if (!strcmp(argv[i], "-IF"))
        {
            imageFile = argv[++i];
        }
        else if (!strcmp(argv[i], "-IR"))
        {
            imageReadPath = argv[++i];
        }
        else if (!strcmp(argv[i], "-TR"))
        {
            terrainReadPath = argv[++i];
        }
        else
        {
            cerr << "Usage:" << endl;
            cerr << "  -I       - Interactive." << endl;
            cerr << "  -D  path - Path to VTKData." << endl;
            cerr << "  -T  path - Image comparison path." << endl;
            cerr << "  -V  file - Image comparison file." << endl;
            cerr << "  -IS path - Path to save image database to." << endl;
            cerr << "  -TS path - Path to save terrain database to." << endl;
            cerr << "  -IR path - Path to read image database from." << endl;
            cerr << "  -TR path - Path to read terrain database from." << endl;
            cerr << "  -IF file - Load JPEG image file." << endl;
            return 1;
        }
    }
    //The globe may contain a multi-resolution geometry source (vtkGeoTerrain), multiple multi-resolution image sources (vtkGeoAlignedImageRepresentation), as well as other representations such as vtkRenderedGraphRepresentation.
    
    // Create the geo view.
    VTK_CREATE(vtkGeoView, view);
    view->DisplayHoverTextOff();
    view->GetRenderWindow()->SetMultiSamples(0);
    view->GetRenderWindow()->SetSize(600,600);
    
    //vtkGeoTerrain contains a multi-resolution tree of geometry representing the globe. It uses a vtkGeoSource subclass to generate the terrain, such as vtkGeoGlobeSource. This source must be set before using the terrain in a vtkGeoView. The terrain also contains an AddActors() method which will update the set of actors representing the globe given the current camera position.
    vtkSmartPointer<vtkGeoTerrain> terrain = vtkSmartPointer<vtkGeoTerrain>::New();
    vtkSmartPointer<vtkGeoSource> terrainSource;
    vtkGeoGlobeSource* globeSource = vtkGeoGlobeSource::New();
    terrainSource.TakeReference(globeSource);
    terrainSource->Initialize();
    terrain->SetSource(terrainSource);
    view->SetTerrain(terrain);
    
    //image anzeigen
    //vtkGeoAlignedImageRepresentation: image tree
    vtkSmartPointer<vtkGeoAlignedImageRepresentation> imageRep =
    vtkSmartPointer<vtkGeoAlignedImageRepresentation>::New();
    vtkSmartPointer<vtkGeoSource> imageSource;
    //It has an associated vtkGeoSource which is responsible for fetching new data. This class keeps the fetched data in a quad-tree structure organized by latitude and longitude.
    /*virtual void vtkGeoSource::RequestChildren	(	vtkGeoTreeNode * 	node	)
    virtual
    Non-blocking methods for to use from the main application. After calling RequestChildren() for a certain node, GetRequestedNodes() will after a certain period of time return a non-null pointer to a collection of four vtkGeoTreeNode objects, which are the four children of the requested node. The collection is reference counted, so you need to eventually call Delete() on the returned collection pointer (if it is non-null).*/
    //vtkGeoSource is an abstract superclass for all multi-resolution data sources shown in a geographic view like vtkGeoView or vtkGeoView2D. vtkGeoSource subclasses need to implement the FetchRoot() method, which fills a vtkGeoTreeNode with the low-res data at the root, and FetchChild(), which produces a refinement of a parent node. Other geovis classes such as vtkGeoTerrain, vtkGeoTerrain2D, and vtkGeoAlignedImageSource use a vtkGeoSource subclass to build their geometry or image caches which are stored in trees. The source itself does not maintain the tree, but simply provides a mechanism for generating refined tree nodes.
    
    //erbt von GeoSource, ist das was wir überschreiben wollen
    //vtkGeoAlignedImageSource* alignedSource = vtkGeoAlignedImageSource::New();
    LLGeoSource *alignedSource = LLGeoSource::New();
    //liest Bild hochauflösendes von der Festplatte
    vtkSmartPointer<vtkJPEGReader> reader =
    vtkSmartPointer<vtkJPEGReader>::New();
    reader->SetFileName(imageFile.c_str());
    reader->Update();
    //übergibt Bild an vtkGeoAlignedImageSource, zerstückelt Bild in kleine Teile
    alignedSource->SetImage(reader->GetOutput());
    
    //void vtkSmartPointer< T >::TakeReference	(	T * 	t	)
    //Transfer ownership of one reference to the given VTK object to this smart pointer. This does not increment the reference count of the object, but will decrement it later. The caller is effectively passing ownership of one reference to the smart pointer. This is useful for code like: vtkSmartPointer<vtkFoo> foo; foo.TakeReference(bar->NewFoo()); The input argument may not be another smart pointer.
    imageSource.TakeReference(alignedSource);
    imageSource->Initialize();
    //vtkGeoAlignedImageRepresentation stellt Baum dem Renderer zur Verfügung
    imageRep->SetSource(imageSource);
    view->AddRepresentation(imageRep);
    
    // Add second image representation
   /* VTK_CREATE(vtkJPEGReader, reader2);
    reader2->SetFileName(image2);
    reader2->Update();
    vtkSmartPointer<vtkGeoAlignedImageSource> imageSource2 =
    vtkSmartPointer<vtkGeoAlignedImageSource>::New();
    imageSource2->SetImage(reader2->GetOutput());
    vtkSmartPointer<vtkGeoAlignedImageRepresentation> imageRep2 =
    vtkSmartPointer<vtkGeoAlignedImageRepresentation>::New();
    imageSource2->Initialize();
    imageRep2->SetSource(imageSource2);
    view->AddRepresentation(imageRep2);
    */
    // Serialize databases
    if (terrainSavePath.length() > 0)
    {
        terrain->SaveDatabase(terrainSavePath.c_str(), 4);
    }
    if (imageSavePath.length() > 0)
    {
        imageRep->SaveDatabase(imageSavePath.c_str());
    }
    
    // Load databases
    if (terrainReadPath.length() > 0)
    {
        terrainSource->ShutDown();
        vtkGeoFileTerrainSource* source = vtkGeoFileTerrainSource::New();
        source->SetPath(terrainReadPath.c_str());
        terrainSource.TakeReference(source);
        terrainSource->Initialize();
    }
    terrain->SetSource(terrainSource);
    if (imageReadPath.length() > 0)
    {
        imageSource->ShutDown();
        vtkGeoFileImageSource* source = vtkGeoFileImageSource::New();
        source->SetPath(imageReadPath.c_str());
        imageSource.TakeReference(source);
        imageSource->Initialize();
    }
    imageRep->SetSource(imageSource);
    
    view->ResetCamera();
    view->GetRenderer()->GetActiveCamera()->Zoom(1.2);
    
    // Add a graph representation
    //Geodaeten
    /*
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
    */
   /* vtkViewTheme* theme = vtkViewTheme::New();
    view->ApplyViewTheme(theme);
    theme->Delete();
    */
    view->Render();
    
    // BUG: Need to call it twice in order to show the imagery on the globe.
    view->Render();
    
    view->GetInteractor()->Initialize();
    view->GetInteractor()->Start();
    
    /*// Delay it for 2 secs.
     vtksys::SystemTools::Delay(10000);
     int retVal = vtkRegressionTestImageThreshold(view->GetRenderWindow(), 11);
     if (retVal == vtkRegressionTester::DO_INTERACTOR)
     {
     // Interact with data.
     view->GetInteractor()->Initialize();
     view->GetInteractor()->Start();
     
     retVal = vtkRegressionTester::PASSED;
     }
     
     // Shut down sources
     terrainSource->ShutDown();
     imageSource->ShutDown();
     imageSource2->ShutDown();
     
     delete [] image;
     delete [] image2;
     return !retVal;*/
}

int main(int argc, char* argv[])
{
    return TestGeoView(argc, argv);
}