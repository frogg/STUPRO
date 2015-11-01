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
#include <vtkCoordinate.h>
#include <vtkRendererCollection.h>

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
        
        
        VTKOpenGL & client = *((VTKOpenGL*)clientData);

        // ((vtkRenderer*)caller)->GetActiveCamera()->GetPosition(cameraPosition);
        // (ClientData*) _clientdata = clientdata;

        
        double cameraPosition[3];
        client.myRenderer->GetActiveCamera()->GetPosition(cameraPosition);
       // client.myRenderer->GetActiveCamera()->GetRoll(); //Azimuth
/*        double cameraViewangle =  client.myRenderer->GetActiveCamera()->GetRoll();
        std::cout << "View Angle: " << cameraViewangle << std::endl;
        double cameraViewangle1 =  client.myRenderer->GetActiveCamera()->GetAzimuth();
        std::cout << "View Angle Azimutz: " << cameraViewangle1 << std::endl;
        double cameraParalled =  client.myRenderer->GetActiveCamera()->GetParallelScale();
        std::cout << "Parallel: " << cameraParalled << std::endl;
*/
        /*
        vtkSmartPointer<vtkCoordinate> coordinate =
                vtkSmartPointer<vtkCoordinate>::New();
        
        
        
        coordinate->SetCoordinateSystemToDisplay();
        coordinate->SetValue(0,0,0);
        
        // This doesn't produce the right value if the sphere is zoomed in???
        
        double* world = coordinate->GetComputedWorldValue(client.myRenderWindow->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
        std::cout << "Middlepoint " << world[0] << ", " << world[1] << ", " << world[2] << std::endl;
         //ViewToWorld
        
        //double world =
        //client.myRenderer->GetViewport()->SetDisplayPoint()
        double direction[3] = {world[0]+100*(world[0]-cameraPosition[0]),world[1]+100*(world[1]-cameraPosition[1]),world[2]+100*(world[2]-cameraPosition[2])};
        
        vtkSmartPointer<vtkPoints> intersectPoints2 = vtkSmartPointer<vtkPoints>::New();
        client.myTree->IntersectWithLine(world, direction, intersectPoints2, NULL);
        
        double intersection2[3];
        
        
            intersectPoints2->GetPoint(0, intersection2);
        Coordinate coord1 = VTKOpenGL::getCoordinates(intersection2);

        
            std::cout << "IntersectionMiddlePoint; " << intersection2[0] << "," << intersection2[1] << "," << intersection2[2] << std::endl;
       */
         /*
        coordinate->SetValue(1,0,0);
        // This doesn't produce the right value if the sphere is zoomed in???
        
        world = coordinate->GetComputedWorldValue(client.myRenderWindow->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
        std::cout << "World coordinate rechts: " << world[0] << ", " << world[1] << ", " << world[2] << std::endl;
        
        
        
        coordinate->SetValue(0,-1,0);
        // This doesn't produce the right value if the sphere is zoomed in???
        
        world = coordinate->GetComputedWorldValue(client.myRenderWindow->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
        std::cout << "World coordinate unten: " << world[0] << ", " << world[1] << ", " << world[2] << std::endl;
        
        
        coordinate->SetValue(-1,0,0);
        // This doesn't produce the right value if the sphere is zoomed in???
        
        world = coordinate->GetComputedWorldValue(client.myRenderWindow->GetInteractor()->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
        std::cout << "World coordinate links: " << world[0] << ", " << world[1] << ", " << world[2] << std::endl;
        std::cout << "Camera: " << cameraPosition[0] << ", " << cameraPosition[1] << ", " << cameraPosition[2] << std::endl;
        
        */
        
        //   std::cout << "View Angle: " << cameraViewangle << std::endl;
        
        double globeOrigin[3] = {0,0,0};
        
        double distanceCameraGlobe = sqrt(pow(cameraPosition[0]-globeOrigin[0], 2)+pow(cameraPosition[1]-globeOrigin[1], 2)+pow(cameraPosition[2]-globeOrigin[2], 2));
    //    std::cout << "Camera Position: " << cameraPosition[0] << ", " << cameraPosition[1] << ", " <<cameraPosition[2]<< "DistanceToCenter" << distanceCameraGlobe << std::endl;

        //double viewEdgeDistance = tan(3.14159265358979323846264338327950288 / 12.0) * distanceCameraGlobe;
        //  std::cout << "View Edge Distance: " << viewEdgeDistance << std::endl;
        

        //   std::cout << "View Edge Distance: " << viewEdgeDistance << std::endl;
        
        
        vtkSmartPointer<vtkPoints> intersectPoints = vtkSmartPointer<vtkPoints>::New();
        
        //  vtkOBBTree * tree = (vtkOBBTree*)clientData;
        
        client.myTree->IntersectWithLine(cameraPosition, globeOrigin, intersectPoints, NULL);
        
        
        
        double intersection[3];
        
        intersectPoints->GetPoint(0, intersection);
        
        Coordinate coord = VTKOpenGL::getCoordinates(intersection);
        std::cout << "Intersection; " << "long: " << coord.longitude << "lat: " << coord.latitude << std::endl;
        /*
        double uppperCorner[3] = {0,viewEdgeDistance,0};
        double lowOrigin[3] = {0,-viewEdgeDistance,0};
        double rightCorner[3] = {0,0,viewEdgeDistance};
        double leftCorner[3] = {0,0,-viewEdgeDistance};
        
        client.myTree->IntersectWithLine(cameraPosition, uppperCorner, intersectPoints, NULL);
        intersectPoints->GetPoint(0, intersection);
        coord = VTKOpenGL::getCoordinates(intersection);
        //     std::cout << "upperCorner; " << "long: " << coord.longitude << "lat: " << coord.latitude << std::endl;
        
        client.myTree->IntersectWithLine(cameraPosition, lowOrigin, intersectPoints, NULL);
        intersectPoints->GetPoint(0, intersection);
        coord = VTKOpenGL::getCoordinates(intersection);
        //    std::cout << "lowOrigin; " << "long: " << coord.longitude << "lat: " << coord.latitude << std::endl;
        
        client.myTree->IntersectWithLine(cameraPosition, rightCorner, intersectPoints, NULL);
        intersectPoints->GetPoint(0, intersection);
        coord = VTKOpenGL::getCoordinates(intersection);
        //   std::cout << "rightCorner; " << "long: " << coord.longitude << "lat: " << coord.latitude << std::endl;
        
        client.myTree->IntersectWithLine(cameraPosition, leftCorner, intersectPoints, NULL);
        intersectPoints->GetPoint(0, intersection);
        coord = VTKOpenGL::getCoordinates(intersection);
     //   std::cout << "leftCorner; " << "long: " << coord.longitude << "lat: " << coord.latitude << std::endl;
         */
        //   std::cout << "leftCorner; " << "long: " << coord.longitude << "lat: " << coord.latitude << std::endl;
        
    /*
        
        //Test 101
        double viewUp[3];
        client.myRenderer->GetActiveCamera()->GetViewUp(viewUp); //ist konstant
        std::cout << "ViewUP" << viewUp[0] << "; " << viewUp[1] << ";" << viewUp[2] << std::endl;
        
        std::cout << "Camera Position: " << cameraPosition[0] << ", " << cameraPosition[1] << ", " <<cameraPosition[2]<< "DistanceToCenter" << distanceCameraGlobe << std::endl;
*/
        
        //Test View Frustum
         double aspect = 1;
         double planes[24];
        
         client.myRenderer->GetActiveCamera()->GetFrustumPlanes(aspect, planes);
        // std::cout << "Left equation" << planes[0] << "*x   " << planes[1] << "*y   " << planes[2] << "*z + " << planes[3]<< std::endl;
     //   std::cout << "Bottom equation" << planes[8] << "*x   " << planes[9] << "*y   " << planes[10] << "*z + " << planes[11]<< std::endl;
        //Get all planes of view frustum
        double planeLeft[4]  = {planes[0],  planes[1], planes[2], planes[3]};
        double planeRight[4]  = {planes[4],  planes[5], planes[6], planes[7]};
        double planeBottom[4]  = {planes[8],  planes[9], planes[10], planes[11]};
        double planeTop[4]  = {planes[12],  planes[13], planes[14], planes[15]};
        double planeFar[4]  = {planes[16],  planes[17], planes[18], planes[19]};
        double planeNear[4]  = {planes[20],  planes[21], planes[22], planes[23]};


        double intersectionVector[3];
        VTKOpenGL::getIntersectionLineFromPlane(planeLeft, planeBottom, intersectionVector);
        std::cout << "IntersectionDirection" << intersectionVector[0] << " ,  " << intersectionVector[1] << "  , " << intersectionVector[2] << std::endl;

        //direction of the line => calculate Point behind earth and then calculate the intersecti
        double pointOnLine[3] = {cameraPosition[0]-100*(intersectionVector[0]),cameraPosition[1]-100*(intersectionVector[1]),cameraPosition[2]-100*(intersectionVector[2])};
        
    
        vtkSmartPointer<vtkPoints> intersectPoints2 = vtkSmartPointer<vtkPoints>::New();
        client.myTree->IntersectWithLine(cameraPosition, pointOnLine, intersectPoints2, NULL);
        
        double intersection2[3];
        intersectPoints2->GetPoint(0, intersection2);
        Coordinate coord1 = VTKOpenGL::getCoordinates(intersection2);
    
        std::cout << "Left; " << intersection2[0] << "," << intersection2[1] << "," << intersection2[2] << std::endl;

        
    };
    
    
    // an artificial Sphere is created as a stand in for our globe in our raycasting Method.
    vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(.5f);
    sphereSource->SetThetaResolution(100);
    sphereSource->SetPhiResolution(100);
    sphereSource->Update();
    
    // the OBBTree allows us to simulate a single raycast inbetween two given points as seen in the clipFunc
    myTree = vtkSmartPointer<vtkOBBTree>::New();
    myTree->SetDataSet(sphereSource->GetOutput());
    myTree->BuildLocator();
    
    
    
    // Create and assign callback for clipping function.
    vtkSmartPointer<vtkCallbackCommand> clipCallback = vtkSmartPointer<vtkCallbackCommand>::New();
    clipCallback->SetCallback(clipFunc);
    
  //  clipCallback->SetClientData(this);
    //myRenderWindow->GetInteractor()->CreateRepeatingTimer(1000);
//    myRenderWindow->GetInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent, clipCallback);
    
    //    myRenderer->GetRenderWindow()->GetInteractor()->CreateRepeatingTimer(17);
    //    myRenderer->AddObserver(vtkCommand::TimerEvent, clipCallback);
    
    
    // Call the function once to correct the clipping range immediately.
    //clipFunc(myRenderer, 0, this, 0);
    clipCallback->SetClientData(this);
    //    clipCallback->SetClientData(myTree);
    //    myRenderer->AddObserver(vtkCommand::MouseMoveEvent, clipCallback);
    //  myRenderWindow->GetInteractor()->CreateRepeatingTimer(17);
    myRenderWindow->GetInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent, clipCallback);
    
    // Call the function once to correct the clipping range immediately.
    //clipFunc(myRenderer, 0, this, 0);
    
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

//not completly correct
//problem falls x2 = 0;
void VTKOpenGL::getIntersectionLineFromPlane(double firstPlane[], double secondPlane[], double lineDirection[]){
   
    double factor =  secondPlane[0]/firstPlane[0];
    secondPlane[0] = 0;
    secondPlane[1] = secondPlane[1] - factor*firstPlane[1];
    secondPlane[2] = secondPlane[2] - factor*firstPlane[2];
    secondPlane[3] = secondPlane[3] - factor*firstPlane[3];
    
    double x2    = 1;
    double x3 = (- secondPlane[3]- secondPlane[1]*x2)/secondPlane[2];
    double x1 = (- firstPlane[3]- firstPlane[1]*x2 - firstPlane[2]*x3)/firstPlane[0];
    
    lineDirection[0] = x1;
    lineDirection[1] = x2;
    lineDirection[2] = x3;
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
    float radius = 0.5f;
    float PI = 3.14159268;
    //coordinate system are not the same as in math formulas
    float x = point[0];
    float y = point[2];
    float z = point[1];
    coordinate.latitude = asin(z/radius) *180/PI;
    coordinate.longitude =  atan2(x, y)  * 180/PI;
    std::cout << "Koordinaten: Lati: " << coordinate.longitude << "Long:" << coordinate.latitude;
        return coordinate;
  
    return coordinate;

}

