/*
 * Main.cpp
 *
 *  Created on: Sep 23, 2015
 *      Author: marukyu
 */

#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkShader2.h"
#include "vtkShaderProgram2.h"
#include "vtkShader2Collection.h"
#include "vtkSmartPointer.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkOpenGLProperty.h"
#include "vtkUniformVariables.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkPNGReader.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkObject.h"
#include "vtkOpenGLTexture.h"

#include "vtkOBBTree.h"

#include <iostream>
#include <fstream>
#include <functional>
#include <cmath>

#include <vtkImageAppendComponents.h>
#include <vtkJPEGReader.h>
#include <vtkImageExtractComponents.h>

std::string readFile(std::string filename)
{
    std::string content;
    std::ifstream file(filename);
    if (!file)
    {
        return content;
    }
    file.seekg(0, std::ios::end);
    content.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&content[0], content.size());
    return content;
}

template<typename T>
class Vector3
{
public:
    Vector3() :
				x(0), y(0), z(0)
    {
    }
    
    Vector3(T x, T y, T z) :
				x(x), y(y), z(z)
    {
    }
    
    Vector3(const T a[]) :
				x(a[0]), y(a[1]), z(a[2])
    {
    }
    
    T * getArray()
    {
        return &x;
    }
    
    const T * getArray() const
    {
        return &x;
    }
    
    T x;
    T y;
    T z;
    
private:
    
    template<typename F>
    Vector3<T> genericOperator(Vector3<T> v, F func) const
    {
        v.x = func(x, v.x);
        v.y = func(y, v.y);
        v.z = func(z, v.z);
        return v;
    }
    
    template<typename F>
    Vector3<T> & genericOperatorAssign(Vector3<T> v, F func)
    {
        x = func(x, v.x);
        y = func(y, v.y);
        z = func(z, v.z);
        return *this;
    }
};

template<typename T>
Vector3<T> operator+(Vector3<T> v, Vector3<T> v2)
{
    return v.genericOperator(v2, std::plus<T>());
}
template<typename T>
Vector3<T> & operator+=(Vector3<T> & v, Vector3<T> v2)
{
    return v.genericOperatorAssign(v2, std::plus<T>());
}
template<typename T>
Vector3<T> operator-(Vector3<T> v, Vector3<T> v2)
{
    return v.genericOperator(v2, std::minus<T>());
}
template<typename T>
Vector3<T> & operator-=(Vector3<T> & v, Vector3<T> v2)
{
    return v.genericOperatorAssign(v2, std::minus<T>());
}
template<typename T>
Vector3<T> operator*(Vector3<T> v, Vector3<T> v2)
{
    return v.genericOperator(v2, std::multiplies<T>());
}
template<typename T>
Vector3<T> & operator*=(Vector3<T> & v, Vector3<T> v2)
{
    return v.genericOperatorAssign(v2, std::multiplies<T>());
}
template<typename T>
Vector3<T> operator/(Vector3<T> v, Vector3<T> v2)
{
    return v.genericOperator(v2, std::divides<T>());
}
template<typename T>
Vector3<T> & operator/=(Vector3<T> & v, Vector3<T> v2)
{
    return v.genericOperatorAssign(v2, std::divides<T>());
}

typedef Vector3<double> Vector3d;

struct Coordinate{
    double latitude;
    double longitude;
};

class VTKOpenGL
{
public:
    
    void run();
    
private:
    
    enum DisplayMode
    {
        DisplayGlobe, DisplayMap
    };
    
    void init();
    void initParameters();
    void initGlobe();
    void initRenderer();
    void initShaders();
    void initCallbacks();
    static Coordinate getCoordinates(double point[]);
    
    vtkSmartPointer<vtkOpenGLTexture> loadAlphaTexture(std::string rgbFile,
                                                       std::string alphaFile) const;
    
    vtkSmartPointer<vtkRenderer> myRenderer;
    vtkSmartPointer<vtkRenderWindow> myRenderWindow;
    vtkSmartPointer<vtkActor> myPlaneActor;
    
    vtkSmartPointer<vtkShader2> myVertexShader;
    vtkSmartPointer<vtkShader2> myFragmentShader;
    
    DisplayMode myDisplayMode;
    
    float myGlobeRadius;
    float myPlaneSize;
    float myDisplayModeInterpolation;
    float myHeightFactor;
};

void VTKOpenGL::run()
{
    init();
    
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
    
    // Create render window with renderer.
    myRenderWindow = vtkRenderWindow::New();
    myRenderWindow->SetWindowName("Famous Globe");
    myRenderWindow->AddRenderer(myRenderer);
    
    // Create interactor for render window.
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<
    vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(myRenderWindow);
    
    // Create interactor style for render window.
    vtkSmartPointer<vtkInteractorStyle> interactorStyle = vtkInteractorStyleTrackballCamera::New();
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
    myVertexShader->GetUniformVariables()->SetUniformf("interpolation", 1,
                                                       &myDisplayModeInterpolation);
    myVertexShader->GetUniformVariables()->SetUniformf("heightOffset", 1, &myHeightFactor);
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
        float range = 2.f;
        ((vtkRenderer*)caller)->ResetCameraClippingRange(-range, range, -range, range, -range, range);
        
        double cameraPosition[3];
        ((vtkRenderer*)caller)->GetActiveCamera()->GetPosition(cameraPosition);
        
        double cameraViewangle =  ((vtkRenderer*)caller)->GetActiveCamera()->GetViewAngle();
        double globeOrigin[3] = {0,0,0};
        
        double distanceCameraGlobe = sqrt(pow(cameraPosition[0]-globeOrigin[0], 2)+pow(cameraPosition[1]-globeOrigin[1], 2)+pow(cameraPosition[2]-globeOrigin[2], 2));
        
        double viewEdgeDistance = tan(15) * distanceCameraGlobe;

        std::cout << "View Edge Distance: " << viewEdgeDistance << std::endl;
        
        
        vtkSmartPointer<vtkPoints> intersectPoints = vtkSmartPointer<vtkPoints>::New();
        
        vtkOBBTree * tree = (vtkOBBTree*)clientData;
        
        tree->IntersectWithLine(cameraPosition, globeOrigin, intersectPoints, NULL);
        
        
        double intersection[3];
        
        intersectPoints->GetPoint(0, intersection);
        
        Coordinate coord = VTKOpenGL::getCoordinates(intersection);
        std::cout << "Intersection; " << "long: " << coord.longitude << "lat: " << coord.latitude << std::endl;
        
        double uppperCorner[3] = {0,viewEdgeDistance,0};
        double lowOrigin[3] = {0,-viewEdgeDistance,0};
        double rightCorner[3] = {viewEdgeDistance,0,0};
        double leftCorner[3] = {-viewEdgeDistance,0,0};
        
        
        
    };
    
    // an artificial Sphere is created as a stand in for our globe in our raycasting Method.
    vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(.5f);
    sphereSource->SetThetaResolution(100);
    sphereSource->SetPhiResolution(100);
    sphereSource->Update();
    
    // the OBBTree allows us to simulate a single raycast inbetween two given points as seen in the clipFunc
    vtkSmartPointer<vtkOBBTree> tree = vtkSmartPointer<vtkOBBTree>::New();
    tree->SetDataSet(sphereSource->GetOutput());
    tree->BuildLocator();
    
    
    // Create and assign callback for clipping function.
    vtkSmartPointer<vtkCallbackCommand> clipCallback = vtkSmartPointer<vtkCallbackCommand>::New();
    clipCallback->SetCallback(clipFunc);
    clipCallback->SetClientData(tree);
    myRenderer->AddObserver(vtkCommand::ResetCameraClippingRangeEvent, clipCallback);
    
    // Call the function once to correct the clipping range immediately.
    clipFunc(myRenderer, 0, tree, 0);
    
    
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
            float effectSpeed = 2.f;
            
            // Smoothly transition interpolation value based on previous and target value.
            client.myDisplayModeInterpolation = (interpolationTarget * effectSpeed +
                                                 client.myDisplayModeInterpolation) / (effectSpeed + 1.f);
            client.myVertexShader->GetUniformVariables()->SetUniformf("interpolation", 1,
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
    auto modeSwitchFunc = [](vtkObject* caller, unsigned long eventId, void* clientData, void* callData)
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

Coordinate VTKOpenGL::getCoordinates(double point[]){
    Coordinate coordinate;
    coordinate.latitude = ((asin(point[0] / .5f)) / 6.28) * 360;
    coordinate.longitude = ((atan2(point[2], point[1])) / 6.28) * 360;
    return coordinate;
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

int main()
{
    VTKOpenGL client;
    
    client.run();
    
    return 0;
}

