#include "VTKOpenGL.hpp"
#include "Utils.hpp"

#include <vtkCommand.h>
#include <vtkCallbackCommand.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkCamera.h>
#include <vtkShaderProgram2.h>
#include <vtkShader2Collection.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageAppendComponents.h>
#include <vtkImageExtractComponents.h>
#include <vtkJPEGReader.h>
#include <vtkObject.h>
#include "vtkSphereSource.h"

void VTKOpenGL::run()
{
	// Perform initialization.
	init();
	
	// Update renderer.
	myRenderWindow->Render();
	
	// Start displaying!
	myRenderWindow->GetInteractor()->Start();
}

void VTKOpenGL::init()
{
	initParameters();
	initGlobe();
	initRenderer();
	initShaders();
	initCallbacks();
}

void VTKOpenGL::initParameters()
{
	// Initialize parameters.
	myDisplayMode = DisplayGlobe;
	myGlobeRadius = 0.5f;
	myPlaneSize = 1.f;
	myDisplayModeInterpolation = 0.f;
	myHeightFactor = 0.05f;
}

void VTKOpenGL::initGlobe()
{
	// Load the texture from RGB and heightmap data.
	vtkSmartPointer<vtkOpenGLTexture> texture = loadAlphaTexture("Resources/bigearth.jpg",
	        "Resources/bigheight.jpg");
	
	// Create plane source for transformation into globe.
	vtkSmartPointer<vtkPlaneSource> plane = vtkPlaneSource::New();
	plane->SetResolution(150, 150);
	
	// Create polygon mapper for plane.
	vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkPolyDataMapper::New();
	planeMapper->SetInputConnection(plane->GetOutputPort());
	
	// Create actor for plane/globe.
	myPlaneActor = vtkActor::New();
	myPlaneActor->SetMapper(planeMapper);
	myPlaneActor->SetTexture(texture);
}

void VTKOpenGL::initRenderer()
{
	// Create renderer with actor for the globe.
	myRenderer = vtkRenderer::New();
	myRenderer->AddActor(myPlaneActor);
	
	// Set camera clipping range.
	float r = myGlobeRadius * 4.f;
	myRenderer->ResetCameraClippingRange( -r, r, -r, r, -r, r);
	
	// Create render window with renderer.
	myRenderWindow = vtkRenderWindow::New();
	myRenderWindow->SetWindowName("Famous Globe");
	myRenderWindow->AddRenderer(myRenderer);
	
	// Create interactor for render window.
	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<
	        vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(myRenderWindow);
	
	// Create interactor style for render window.
	vtkSmartPointer<vtkInteractorStyle> interactorStyle = StuproInteractor::New();
	interactorStyle->SetAutoAdjustCameraClippingRange(false);
	interactor->SetInteractorStyle(interactorStyle);
}

void VTKOpenGL::initShaders()
{
	// Create shader program.
	vtkSmartPointer<vtkShaderProgram2> shaderProgram = vtkShaderProgram2::New();
	shaderProgram->SetContext(myRenderWindow);
	
	// Create and load fragment shader.
	myFragmentShader = vtkShader2::New();
	myFragmentShader->SetType(VTK_SHADER_TYPE_FRAGMENT);
	myFragmentShader->SetSourceCode(readFile("Shader/TestShader.fsh").c_str());
	myFragmentShader->SetContext(shaderProgram->GetContext());
	
	// Create and load vertex shader.
	myVertexShader = vtkShader2::New();
	myVertexShader->SetType(VTK_SHADER_TYPE_VERTEX);
	myVertexShader->SetSourceCode(readFile("Shader/TestShader.vsh").c_str());
	myVertexShader->SetContext(shaderProgram->GetContext());
	
	// TODO: Find a way to get texture ID (VTK's GetTextureUnit() always returns 0).
	int textureID = 0;
	myFragmentShader->GetUniformVariables()->SetUniformi("texture", 1, &textureID);
	
	// Assign uniform variables.
	myVertexShader->GetUniformVariables()->SetUniformf("globeRadius", 1, &myGlobeRadius);
	myVertexShader->GetUniformVariables()->SetUniformf("planeSize", 1, &myPlaneSize);
	myVertexShader->GetUniformVariables()->SetUniformf("displayMode", 1,
	        &myDisplayModeInterpolation);
	myVertexShader->GetUniformVariables()->SetUniformf("heightFactor", 1, &myHeightFactor);
	myVertexShader->GetUniformVariables()->SetUniformi("heightTexture", 1, &textureID);
	
	// Add shaders to shader program.
	shaderProgram->GetShaders()->AddItem(myFragmentShader);
	shaderProgram->GetShaders()->AddItem(myVertexShader);
	
	// Add shader to globe actor.
	vtkSmartPointer<vtkOpenGLProperty> openGLproperty =
	        static_cast<vtkOpenGLProperty*>(myPlaneActor->GetProperty());
	openGLproperty->SetPropProgram(shaderProgram);
	openGLproperty->ShadingOn();
}

void VTKOpenGL::initCallbacks()
{
    
    // Create callback function that corrects the camera clipping range to work around a VTK bug.
    auto clipFunc = [](vtkObject* caller, unsigned long eventId, void* clientData, void* callData)
    {
    //   float range = 2.f;
      //  ((vtkRenderer*)caller)->ResetCameraClippingRange(-range, range, -range, range, -range, range);
        

        
    

        
        double cameraPosition[3];
<<<<<<< HEAD
        // ((vtkRenderer*)caller)->GetActiveCamera()->GetPosition(cameraPosition);
        ClientData* _clientdata = (ClientData*)clientData;
        _clientdata->renderer->GetActiveCamera()->GetPosition(cameraPosition);
        
        
        double cameraViewangle =  ((vtkRenderer*)caller)->GetActiveCamera()->GetViewAngle();
=======
        
        VTKOpenGL & client = *((VTKOpenGL*)clientData);
       // ((vtkRenderer*)caller)->GetActiveCamera()->GetPosition(cameraPosition);
       // (ClientData*) _clientdata = clientdata;
        
        client.myRenderer->GetActiveCamera()->GetPosition(cameraPosition);
        double cameraViewangle =  client.myRenderer->GetActiveCamera()->GetViewAngle();
>>>>>>> 3d1162b0804599e0543b2c7d77881b7d1a589f0c
        double globeOrigin[3] = {0,0,0};
        
        double distanceCameraGlobe = sqrt(pow(cameraPosition[0]-globeOrigin[0], 2)+pow(cameraPosition[1]-globeOrigin[1], 2)+pow(cameraPosition[2]-globeOrigin[2], 2));
        
        double viewEdgeDistance = tan(3.14159265358979323846264338327950288 / 12.0) * distanceCameraGlobe;
        
        std::cout << "View Edge Distance: " << viewEdgeDistance << std::endl;
        
        
        vtkSmartPointer<vtkPoints> intersectPoints = vtkSmartPointer<vtkPoints>::New();
        
      //  vtkOBBTree * tree = (vtkOBBTree*)clientData;
<<<<<<< HEAD
        vtkOBBTree * tree = _clientdata->tree;
        tree->IntersectWithLine(cameraPosition, globeOrigin, intersectPoints, NULL);
=======
        client.myTree->IntersectWithLine(cameraPosition, globeOrigin, intersectPoints, NULL);
>>>>>>> 3d1162b0804599e0543b2c7d77881b7d1a589f0c
        
        asin(0.5);
        atan(0.5);
        
        double intersection[3];
        
        intersectPoints->GetPoint(0, intersection);
        
        Coordinate coord = VTKOpenGL::getCoordinates(intersection);
        std::cout << "Intersection; " << "long: " << coord.longitude << "lat: " << coord.latitude << std::endl;
        
        double uppperCorner[3] = {0,viewEdgeDistance,0};
        double lowOrigin[3] = {0,-viewEdgeDistance,0};
        double rightCorner[3] = {0,0,viewEdgeDistance};
        double leftCorner[3] = {0,0,-viewEdgeDistance};
        
         client.myTree->IntersectWithLine(cameraPosition, uppperCorner, intersectPoints, NULL);
        intersectPoints->GetPoint(0, intersection);
        coord = VTKOpenGL::getCoordinates(intersection);
        std::cout << "upperCorner; " << "long: " << coord.longitude << "lat: " << coord.latitude << std::endl;
        
         client.myTree->IntersectWithLine(cameraPosition, lowOrigin, intersectPoints, NULL);
        intersectPoints->GetPoint(0, intersection);
        coord = VTKOpenGL::getCoordinates(intersection);
        std::cout << "lowOrigin; " << "long: " << coord.longitude << "lat: " << coord.latitude << std::endl;
        
         client.myTree->IntersectWithLine(cameraPosition, rightCorner, intersectPoints, NULL);
        intersectPoints->GetPoint(0, intersection);
        coord = VTKOpenGL::getCoordinates(intersection);
        std::cout << "rightCorner; " << "long: " << coord.longitude << "lat: " << coord.latitude << std::endl;
        
         client.myTree->IntersectWithLine(cameraPosition, leftCorner, intersectPoints, NULL);
        intersectPoints->GetPoint(0, intersection);
        coord = VTKOpenGL::getCoordinates(intersection);
        std::cout << "leftCorner; " << "long: " << coord.longitude << "lat: " << coord.latitude << std::endl;


    };

    
    // an artificial Sphere is created as a stand in for our globe in our raycasting Method.
    vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(.5f);
    sphereSource->SetThetaResolution(100);
    sphereSource->SetPhiResolution(100);
    sphereSource->Update();
    
    // the OBBTree allows us to simulate a single raycast inbetween two given points as seen in the clipFunc
    vtkSmartPointer<vtkOBBTree> myTree = vtkSmartPointer<vtkOBBTree>::New();
    myTree->SetDataSet(sphereSource->GetOutput());
    myTree->BuildLocator();
    
    clientdata->tree = myTree;
    clientdata->renderer = myRenderer;

    
    // Create and assign callback for clipping function.
    vtkSmartPointer<vtkCallbackCommand> clipCallback = vtkSmartPointer<vtkCallbackCommand>::New();
    clipCallback->SetCallback(clipFunc);

    clipCallback->SetClientData(clientdata);
    //myRenderWindow->GetInteractor()->CreateRepeatingTimer(1000);
    myRenderWindow->GetInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent, clipCallback);

//    myRenderer->GetRenderWindow()->GetInteractor()->CreateRepeatingTimer(17);
//    myRenderer->AddObserver(vtkCommand::TimerEvent, clipCallback);

    
    // Call the function once to correct the clipping range immediately.
    clipFunc(myRenderer, 0, clientdata, 0);
    clipCallback->SetClientData(this);
//    clipCallback->SetClientData(myTree);
//    myRenderer->AddObserver(vtkCommand::MouseMoveEvent, clipCallback);
  //  myRenderWindow->GetInteractor()->CreateRepeatingTimer(17);
    myRenderWindow->GetInteractor()->AddObserver(vtkCommand::MouseMoveEvent, clipCallback);
    
    // Call the function once to correct the clipping range immediately.
    clipFunc(myRenderer, 0, this, 0);
    
	// Create callback function that corrects the camera clipping range to work around a VTK bug.
	auto timerFunc = [](vtkObject* caller, unsigned long eventId, void* clientData, void* callData)
	{
        
		VTKOpenGL & client = *((VTKOpenGL*)clientData);

		// Determine target interpolation based on display mode.
		    float interpolationTarget = client.myDisplayMode == DisplayGlobe ? 0.f : 1.f;

		    // Check if change is significant enough to be re-rendered.
		    if (std::abs(interpolationTarget - client.myDisplayModeInterpolation) > 0.000001f)
		    {	
			    // Controls the speed of the globe-map transition.
			    float effectSpeed = .2f;

			    // Smoothly transition interpolation value based on previous and target value.
			    client.myDisplayModeInterpolation = (interpolationTarget * effectSpeed +
					    client.myDisplayModeInterpolation) / (effectSpeed + 1.f);
			    client.myVertexShader->GetUniformVariables()->SetUniformf("displayMode", 1,
					    &client.myDisplayModeInterpolation);

			    // Update renderer.
			    client.myRenderWindow->Render();
		    }
	    };
	
	// Create and assign callback for clipping function.
	vtkSmartPointer<vtkCallbackCommand> timerCallback = vtkSmartPointer<vtkCallbackCommand>::New();
	timerCallback->SetCallback(timerFunc);
	timerCallback->SetClientData(this);
	
	// Enable timer on the render window.
	myRenderWindow->GetInteractor()->CreateRepeatingTimer(17);
	myRenderWindow->GetInteractor()->AddObserver(vtkCommand::TimerEvent, timerCallback);
	
	// Create callback function to switch display modes (using the '1' and '2' keys)
	auto modeSwitchFunc =
	        [](vtkObject* caller, unsigned long eventId, void* clientData, void* callData)
	        {	
		        VTKOpenGL & client = *((VTKOpenGL*)clientData);

		        // Get the interactor to determine the pressed key.
		        vtkRenderWindowInteractor * interactor = (vtkRenderWindowInteractor*)caller;

		        if (interactor->GetKeyCode() == 49)// 1 key
		        {	
			        client.myDisplayMode = DisplayGlobe;
		        }
		        else if (interactor->GetKeyCode() == 50) // 2 key
		        {	
			        client.myDisplayMode = DisplayMap;
		        }
	        };
	
	// Create and assign callback for mode switch function.
	vtkSmartPointer<vtkCallbackCommand> modeSwitchCallback =
	        vtkSmartPointer<vtkCallbackCommand>::New();
	modeSwitchCallback->SetCallback(modeSwitchFunc);
	modeSwitchCallback->SetClientData(this);
	myRenderWindow->GetInteractor()->AddObserver(vtkCommand::KeyPressEvent, modeSwitchCallback);
}

vtkSmartPointer<vtkOpenGLTexture> VTKOpenGL::loadAlphaTexture(std::string rgbFile,
        std::string alphaFile) const
{
	// Read RGB data from file.
	vtkSmartPointer<vtkJPEGReader> rgbReader = vtkSmartPointer<vtkJPEGReader>::New();
	rgbReader->SetFileName(rgbFile.c_str());
	
	// Read alpha data from file.
	vtkSmartPointer<vtkJPEGReader> alphaReader = vtkSmartPointer<vtkJPEGReader>::New();
	alphaReader->SetFileName(alphaFile.c_str());
	
	// Get red channel from RGB data.
	vtkSmartPointer<vtkImageExtractComponents> extractRedFilter = vtkSmartPointer<
	        vtkImageExtractComponents>::New();
	extractRedFilter->SetInputConnection(rgbReader->GetOutputPort());
	extractRedFilter->SetComponents(0);
	extractRedFilter->Update();
	
	// Get green channel from RGB data.
	vtkSmartPointer<vtkImageExtractComponents> extractGreenFilter = vtkSmartPointer<
	        vtkImageExtractComponents>::New();
	extractGreenFilter->SetInputConnection(rgbReader->GetOutputPort());
	extractGreenFilter->SetComponents(1);
	extractGreenFilter->Update();
	
	// Get blue channel from RGB data.
	vtkSmartPointer<vtkImageExtractComponents> extractBlueFilter = vtkSmartPointer<
	        vtkImageExtractComponents>::New();
	extractBlueFilter->SetInputConnection(rgbReader->GetOutputPort());
	extractBlueFilter->SetComponents(2);
	extractBlueFilter->Update();
	
	// Get any channel from alpha data.
	vtkSmartPointer<vtkImageExtractComponents> extractHeightFilter = vtkSmartPointer<
	        vtkImageExtractComponents>::New();
	extractHeightFilter->SetInputConnection(alphaReader->GetOutputPort());
	extractHeightFilter->SetComponents(0);
	extractHeightFilter->Update();
	
	// Combine channels.
	vtkSmartPointer<vtkImageAppendComponents> appendFilter = vtkSmartPointer<
	        vtkImageAppendComponents>::New();
	appendFilter->SetInputConnection(0, extractRedFilter->GetOutputPort());
	appendFilter->AddInputConnection(0, extractGreenFilter->GetOutputPort());
	appendFilter->AddInputConnection(0, extractBlueFilter->GetOutputPort());
	appendFilter->AddInputConnection(0, extractHeightFilter->GetOutputPort());
	appendFilter->Update();
	
	// Load texture from combined image data.
	vtkSmartPointer<vtkOpenGLTexture> texture = vtkSmartPointer<vtkOpenGLTexture>::New();
	texture->SetInputConnection(appendFilter->GetOutputPort());
	return texture;
	
}

Coordinate VTKOpenGL::getCoordinates(double point[]){
        Coordinate coordinate;
        coordinate.latitude = ((asin(point[0] / .5f)) / 6.28) * 360;
        coordinate.longitude = ((atan2(point[2], point[1])) / 6.28) * 360;
        return coordinate;
}

