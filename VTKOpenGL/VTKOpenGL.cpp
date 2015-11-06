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


    // create an additional plane and sphere to evaluate the tiles that are visible

    vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(myGlobeRadius);
    sphereSource->SetThetaResolution(100);
    sphereSource->SetPhiResolution(100);
    sphereSource->Update();

    // the OBBTree allows us to simulate a single raycast inbetween two given points as seen in the clipFunc
    mySphereTree = vtkSmartPointer<vtkOBBTree>::New();
    mySphereTree->SetDataSet(sphereSource->GetOutput());
    mySphereTree->BuildLocator();

    // an artificial Plane to calculate raycasting coordinates
    /*vtkSmartPointer<vtkPlaneSource> planeSource = vtkSmartPointer<vtkPlaneSource>::New();
    planeSource->SetOrigin(-2, -1, 0);
    planeSource->SetPoint1(2, -1, 0);
    planeSource->SetPoint2(-2, 1, 0);
    planeSource->Update();*/

    myPlaneTree = vtkSmartPointer<vtkOBBTree>::New();
    myPlaneTree->SetDataSet(plane->GetOutput());
    myPlaneTree->BuildLocator();
}

void VTKOpenGL::initRenderer()
{
    // Create renderer with actor for the globe.
    myRenderer = vtkRenderer::New();
    myRenderer->AddActor(myPlaneActor);

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

        std::vector<Coordinate> intersectionCoordinates;
        if (client.myDisplayMode == DisplayGlobe) {
            std::vector<double [3]> worldIntersectionPoints = VTKOpenGL::getIntersectionPoints (planes, cameraPosition, client.mySphereTree);
            intersectionCoordinates = VTKOpenGL::getGlobeCoordinates (worldIntersectionPoints, client.myGlobeRadius);
        } else {
            std::vector<double [3]> worldIntersectionPoints = VTKOpenGL::getIntersectionPoints(planes, cameraPosition, client.myPlaneTree);
            intersectionCoordinates = VTKOpenGL::getPlaneCoordinates (worldIntersectionPoints, 4, 2);
        }
    };



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

        switch (interactor->GetKeyCode())// 1 key
        {
        case 49: // '1'-Key
            client.myDisplayMode = DisplayGlobe;
            break;
        case 50: // '2'-Key
            client.myDisplayMode = DisplayMap;
            break;
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

std::vector<double[3]> VTKOpenGL::getIntersectionPoints(double planes[], double cameraPosition[], vtkSmartPointer<vtkOBBTree> tree)
{
    // left, right, bottom, top, near, far
    double planeArray[6][4];
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 4; j++) {
            planeArray[i][j] = planes[4 * i + j];
        }
    }

    std::vector<double [3]> worldIntersectionPoints;
    for (int j = 0; j < 4; j++) {
        double intersection[3];
//        VTKOpenGL::getIntersectionPoint(planeLeft, planeBottom, planeFar, cameraPosition,tree,intersection[0]);

        VTKOpenGL::getIntersectionPoint(planeArray[j % 2], planeArray[j / 2 + 2], planeArray[5], cameraPosition, tree, intersection);
        worldIntersectionPoints.push_back(intersection);
    }

    return worldIntersectionPoints;
}

std::vector<Coordinate> VTKOpenGL::getGlobeCoordinates(std::vector<double[3]> worldPoints, double radius)
{
    std::vector<Coordinate> globeCoordinates;
    for (double *worldCoordinate : worldPoints) {
        globeCoordinates.push_back (Coordinate::getCoordinatesFromGlobePoint(worldCoordinate, radius));
    }
    return globeCoordinates;
}

std::vector<Coordinate> VTKOpenGL::getPlaneCoordinates(std::vector<double[3]> worldPoints, double planeWidth, double planeHeight)
{
    std::vector<Coordinate> globeCoordinates;
    for (double *worldCoordinate : worldPoints) {
        globeCoordinates.push_back (Coordinate::getCoordinatesFromPlanePoint(worldCoordinate[0], worldCoordinate[1], planeWidth, planeHeight));
    }
    return globeCoordinates;
}


/*void VTKOpenGL::getGlobeCoordinates(std::vector<Coordinate> &coordinates, vtkSmartPointer<vtkOBBTree> tree, double cameraPosition[], double planes[24]){
    
    //Get all planes of view frustum


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
}*/





