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
#include "vtkLight.h"
#include <vtkTransformTextureCoords.h>
#include <vtkTexture.h>
#include <vtkTextureMapToSphere.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader.h>
#include <vtkTexturedSphereSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include "vtkCamera.h"
// Define interaction style
vtkSmartPointer<vtkRenderer> renderer;
class KeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static KeyPressInteractorStyle* New();
    vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnKeyPress() override
    {
        // Get the keypress
        vtkRenderWindowInteractor *rwi = this->Interactor;
        std::string key = rwi->GetKeySym();
        
        // Output the key that was pressed
        std::cout << "Pressed " << key << std::endl;
        
        // Handle an arrow key
        if(key == "Up")
        {
            std::cout << "The up arrow was pressed." << std::endl;
        }
        
        // Handle a "normal" key
        if(key == "a")
        {
            std::cout << "The a key was pressed." << std::endl;
            renderer->GetActiveCamera()->SetPosition(1, 1, 1);
        }
        
        // Forward events
        vtkInteractorStyleTrackballCamera::OnKeyPress();
    }
    
};
vtkStandardNewMacro(KeyPressInteractorStyle);

int main(int argc, char *argv[])
{
    
    
    argv[1]="/Users/razerpwnz/Development/GIT/GeoViewTest/world.jpg";
    if (argc < 0)
    {
        std::cout << "Usage: " << argv[0]
        << " texture(.png)"
        << " [translate]" <<std::endl;
        return EXIT_FAILURE;
    }
    double translate[3];
    if (argc > 2)
    {
        translate[0] = atof(argv[2]);
    }
    else
    {
        translate[0] = 0.0;
    }
    translate[1] = 0.0;
    translate[2] = 0.0;
    std::cout << translate[0] << ", "
    << translate[1] << ", "
    << translate[2] << "\n";
    // Create a sphere with texture coordinates
    vtkSmartPointer<vtkTexturedSphereSource> source =
    vtkSmartPointer<vtkTexturedSphereSource>::New();
    source->SetPhiResolution(500);
    source->SetThetaResolution(500);
    // Read texture file
    vtkSmartPointer<vtkImageReader2Factory> readerFactory =
    vtkSmartPointer<vtkImageReader2Factory>::New();
    vtkImageReader2 *imageReader =
    readerFactory->CreateImageReader2(argv[1]);
    imageReader->SetFileName(argv[1]);
    
    // Create texture
    vtkSmartPointer<vtkTexture> texture =
    vtkSmartPointer<vtkTexture>::New();
    texture->SetInputConnection(imageReader->GetOutputPort());
    
    vtkSmartPointer<vtkTransformTextureCoords> transformTexture =
    vtkSmartPointer<vtkTransformTextureCoords>::New();
    transformTexture->SetInputConnection(source->GetOutputPort());
    transformTexture->SetPosition(translate);
    
    vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(transformTexture->GetOutputPort());
    
    vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
    actor->SetMapper( mapper );
    actor->SetTexture( texture );
   // actor->GetProperty()->SetColor(1,0,0);
   // actor->GetProperty()->SetAmbient(0.3);
   // actor->GetProperty()->SetDiffuse(0.0);
   // actor->GetProperty()->SetSpecular(1.0);
   // actor->GetProperty()->SetSpecularPower(5.0);
    
    
    // A renderer and render window
    renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    
    // An interactor
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    
    vtkSmartPointer<KeyPressInteractorStyle> style =
    vtkSmartPointer<KeyPressInteractorStyle>::New();
    renderWindowInteractor->SetInteractorStyle(style);
    style->SetCurrentRenderer(renderer);
    
    renderer->AddActor(actor);
    renderer->SetBackground(0.1,0.2,0.4); // Background color white
    
    vtkSmartPointer<vtkLight> light =
    vtkSmartPointer<vtkLight>::New();
    light->SetFocalPoint(1.875,0.6125,0);
    light->SetPosition(0.875,1.6125,1);
    renderer->AddLight(light);
    
    renderWindow->Render();
    
    renderWindowInteractor->Start();
    
    return EXIT_SUCCESS;
}