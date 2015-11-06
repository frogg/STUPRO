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

#include <Eigen-v3.2.6/Dense>

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

    for (int i = 0; i != 4; i++) {
        myRenderer->AddActor(myFrustum.getActors()[i]);
    }

    // Set camera clipping range.
    float r = myGlobeRadius * 5.f;
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
        
        VTKOpenGL & client = *((VTKOpenGL*)clientData);
        //get current position of the camera
        double cameraPosition[3];
        client.myRenderer->GetActiveCamera()->GetPosition(cameraPosition);

        //get aspect, check this, if it also works on tiled displays
         double aspect = client.myRenderer->GetTiledAspectRatio();
        //get view frustum
         double planes[24];
         client.myRenderer->GetActiveCamera()->GetFrustumPlanes(aspect, planes);
        
        std::vector<Coordinate> coordinates;
        client.getCoordinates(coordinates, client.myTree, cameraPosition, planes);
        
        
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

        double planes[24];

        switch (interactor->GetKeyCode())// 1 key
        {
        case 49: // '1'-Key
            client.myDisplayMode = DisplayGlobe;
            break;
        case 50: // '2'-Key
            client.myDisplayMode = DisplayMap;
            break;
        case 52: // '4'-Key
            client.myRenderer->GetActiveCamera()->GetFrustumPlanes(1, planes);
            client.myFrustum.updatePlanes(planes);
            break;
        case 54:
     /*       client.myRenderer->GetActiveCamera()->GetFrustumPlanes(1, planes);
            Coordinate coordinates[5];
            double cameraPosition[3];
            client.myRenderer->GetActiveCamera()->GetPosition(cameraPosition);
            client.getCoordinates(coordinates, client.myTree, cameraPosition, planes); */
            break;
      
        case 53:

            client.myRenderer->GetActiveCamera()->GetFrustumPlanes(1, planes);

#define planeLeft    {planes[0],  planes[1], planes[2], planes[3]}
#define planeRight   {planes[4],  planes[5], planes[6], planes[7]}
#define planeBottom  {planes[8],  planes[9], planes[10], planes[11]}
#define planeTop     {planes[12],  planes[13], planes[14], planes[15]}
#define planeFar     {planes[20],  planes[21], planes[22], planes[23]}

            double planeArrays[4][3][4] = {
                {
                    planeLeft, planeBottom, planeFar
                }, {
                    planeLeft, planeTop, planeFar
                }, {
                    planeRight, planeTop, planeFar
                }, {
                    planeRight, planeBottom, planeFar
                }
            };

            double planePoints[4][3][3];

            for (int i = 0; i != 4; i++) {
                client.cutPlanes (planeArrays[i], planePoints[i][0]);
                client.cutPlanes (planeArrays[(i + 1) % 4], planePoints[i][1]);

                client.myRenderer->GetActiveCamera()->GetPosition(planePoints[i][2]);
            }

            client.myFrustum.updatePlanes(planePoints);

            break;
#undef planeLeft
#undef planeRight
#undef planeBottom
#undef planeTop
#undef planeFar
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



void VTKOpenGL::cutPlanes(double planes[3][4], double cut [3]) {
    Eigen::Matrix3d planeMatrix;
    Eigen::Vector3d offset;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            planeMatrix(i, j) = planes[i][j];
        }
        offset(i) = -planes[i][3];
    }

    Eigen::Vector3d cutPoint = planeMatrix.colPivHouseholderQr().solve(offset);//.lu().solve(offset);

    // copy return value to avoid memory issues
    for (int i = 0; i < 3; i++) {
        cut[i] = cutPoint(i);
    }
}

void VTKOpenGL::getIntersectionPoint(double plane1[4], double plane2[4], double plane3[4], double cameraPosition[3],vtkSmartPointer<vtkOBBTree> tree, double intersection[3]){

    double planes[3][4];
    double intersectionOfPlanes[3];
    for (int i = 0; i < 4; i++) {
        planes[0][i] = plane1[i];
        planes[1][i] = plane2[i];
        planes[2][i] = plane3[i];
    }

    cutPlanes(planes, intersectionOfPlanes);

    vtkSmartPointer<vtkPoints> intersectPoint = vtkSmartPointer<vtkPoints>::New();
    tree->IntersectWithLine(cameraPosition, intersectionOfPlanes, intersectPoint, NULL);

    if(intersectPoint->GetNumberOfPoints() > 0) {
        intersectPoint->GetPoint(0, intersection);
    } else {
        for (int i = 0; i < 3; i++) {
            intersection[i] = 0;
        }
    }
}
void VTKOpenGL::getCoordinates(std::vector<Coordinate> &coordinates, vtkSmartPointer<vtkOBBTree> tree, double cameraPosition[], double planes[24]){
    
    //Get all planes of view frustum
    double planeLeft[4]  = {planes[0],  planes[1], planes[2], planes[3]};
    double planeRight[4]  = {planes[4],  planes[5], planes[6], planes[7]};
    double planeBottom[4]  = {planes[8],  planes[9], planes[10], planes[11]};
    double planeTop[4]  = {planes[12],  planes[13], planes[14], planes[15]};
    double planeFar[4]  = {planes[20],  planes[21], planes[22], planes[23]};

    double intersection[4][3];
    VTKOpenGL::getIntersectionPoint(planeLeft, planeBottom, planeFar, cameraPosition,tree,intersection[0]);
    coordinates.push_back(Coordinate::getCoordinatesFromGlobePoint(intersection[0],myGlobeRadius));
    VTKOpenGL::getIntersectionPoint(planeRight, planeBottom, planeFar, cameraPosition,tree, intersection[1]);
    coordinates.push_back(Coordinate::getCoordinatesFromGlobePoint(intersection[1],myGlobeRadius));
    VTKOpenGL::getIntersectionPoint(planeLeft, planeTop, planeFar, cameraPosition,tree, intersection[2]);
    coordinates.push_back(Coordinate::getCoordinatesFromGlobePoint(intersection[2],myGlobeRadius));
    VTKOpenGL::getIntersectionPoint(planeRight, planeTop, planeFar, cameraPosition,tree, intersection[3]);
    coordinates.push_back(Coordinate::getCoordinatesFromGlobePoint(intersection[3],myGlobeRadius));

    double globeOrigin[3] = {0,0,0};

    vtkSmartPointer<vtkPoints> intersectPoints = vtkSmartPointer<vtkPoints>::New();
    tree->IntersectWithLine(cameraPosition, globeOrigin, intersectPoints, NULL);
    double centerPoint[3];
    intersectPoints->GetPoint(0, centerPoint);
    coordinates.push_back(Coordinate::getCoordinatesFromGlobePoint(centerPoint,myGlobeRadius));
    Coordinate::logCoordinates(coordinates);
}





