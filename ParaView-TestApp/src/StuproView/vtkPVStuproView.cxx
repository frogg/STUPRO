#include "vtkPVStuproView.h"
#include "vtkObjectFactory.h"

#include "vtkSmartPointer.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkShader2.h"
#include "vtkShaderProgram2.h"
#include "vtkShader2Collection.h"
#include "vtkUniformVariables.h"
#include "vtkOpenGLProperty.h"

#include "vtkJPEGReader.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageAppendComponents.h"
#include "vtkOpenGLTexture.h"
#include "vtkCallbackCommand.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyle.h"
#include "vtkInteractorStyleTerrain.h"

#include <iostream>
#include <fstream>
#include <functional>

vtkStandardNewMacro(vtkPVStuproView);
//----------------------------------------------------------------------------
vtkPVStuproView::vtkPVStuproView()
{
}

//----------------------------------------------------------------------------
vtkPVStuproView::~vtkPVStuproView()
{
}

//----------------------------------------------------------------------------
void vtkPVStuproView::Initialize(unsigned int id)
{
	this->Superclass::Initialize(id);

	initParameters();
	initGlobe();
	initRenderer();
	initShaders();
	initCallbacks();
}

//----------------------------------------------------------------------------
void vtkPVStuproView::initParameters()
{
	// Initialize parameters.
	myDisplayMode = DisplayGlobe;
	myGlobeRadius = 0.5f;
	myPlaneSize = 1.f;
	myDisplayModeInterpolation = 0.f;
	myHeightFactor = 0.05f;
}

//----------------------------------------------------------------------------
void vtkPVStuproView::initGlobe()
{
	// Load the texture from RGB and heightmap data.
	vtkSmartPointer<vtkOpenGLTexture> texture = loadAlphaTexture("Resources/bigearth.jpg", "Resources/bigheight.jpg");

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

//----------------------------------------------------------------------------
void vtkPVStuproView::initRenderer()
{
	// Create renderer with actor for the globe.
	// myRenderer = vtkRenderer::New(); // not needed here
	GetRenderer()->AddActor(myPlaneActor);

	// needed later for own interactor style
	// Create interactor for render window.
	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<
		vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(GetRenderWindow());

	// Create interactor style for render window.
	vtkSmartPointer<vtkInteractorStyle> interactorStyle = vtkInteractorStyleTerrain::New();
	interactor->SetInteractorStyle(interactorStyle);
}

//----------------------------------------------------------------------------
void vtkPVStuproView::initShaders()
{
	// Create shader program.
	vtkSmartPointer<vtkShaderProgram2> shaderProgram = vtkShaderProgram2::New();
	shaderProgram->SetContext(GetRenderWindow());

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

//----------------------------------------------------------------------------
void vtkPVStuproView::initCallbacks()
{
	// Create callback function that corrects the camera clipping range to work around a VTK bug.
	auto clipFunc = [](vtkObject* caller, unsigned long eventId, void* clientData, void* callData)
	{
		float range = 2.f;
		((vtkRenderer*) caller)->ResetCameraClippingRange(-range, range, -range, range, -range, range);
	};

	// Create and assign callback for clipping function.
	vtkSmartPointer<vtkCallbackCommand> clipCallback = vtkSmartPointer<vtkCallbackCommand>::New();
	clipCallback->SetCallback(clipFunc);
	GetRenderer()->AddObserver(vtkCommand::ResetCameraClippingRangeEvent, clipCallback);

	// Call the function once to correct the clipping range immediately.
	clipFunc(GetRenderer(), 0, 0, 0);

	// Create callback function that corrects the camera clipping range to work around a VTK bug.
	auto timerFunc = [](vtkObject* caller, unsigned long eventId, void* clientData, void* callData)
	{
		vtkPVStuproView& client = *((vtkPVStuproView*) clientData);

		// Determine target interpolation based on display mode.
		float interpolationTarget = client.myDisplayMode == DisplayGlobe ? 0.f : 1.f;

		// Check if change is significant enough to be re-rendered.
		if(std::abs(interpolationTarget - client.myDisplayModeInterpolation) > 0.000001f)
		{
			// Controls the speed of the globe-map transition.
			float effectSpeed = 2.f;

			// Smoothly transition interpolation value based on previous and target value.
			client.myDisplayModeInterpolation = (interpolationTarget * effectSpeed +
												 client.myDisplayModeInterpolation) / (effectSpeed + 1.f);
			client.myVertexShader->GetUniformVariables()->SetUniformf("interpolation", 1,
																	  &client.myDisplayModeInterpolation);

			// Update renderer.
			client.GetRenderWindow()->Render();
		}
	};

	// Create and assign callback for clipping function.
	vtkSmartPointer<vtkCallbackCommand> timerCallback = vtkSmartPointer<vtkCallbackCommand>::New();
	timerCallback->SetCallback(timerFunc);
	timerCallback->SetClientData(this);

	// Enable timer on the render window.
	GetRenderWindow()->GetInteractor()->CreateRepeatingTimer(17);
	GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::TimerEvent, timerCallback);

	// Create callback function to switch display modes (using the '1' and '2' keys)
	auto modeSwitchFunc = [](vtkObject* caller, unsigned long eventId, void* clientData, void* callData)
	{
		vtkPVStuproView & client = *((vtkPVStuproView*) clientData);

		// Get the interactor to determine the pressed key.
		vtkRenderWindowInteractor * interactor = (vtkRenderWindowInteractor*) caller;

		if(interactor->GetKeyCode() == 49)// 1 key
		{
			client.myDisplayMode = DisplayGlobe;
		}
		else if(interactor->GetKeyCode() == 50) // 2 key
		{
			client.myDisplayMode = DisplayMap;
		}
	};

	// Create and assign callback for mode switch function.
	vtkSmartPointer<vtkCallbackCommand> modeSwitchCallback =
		vtkSmartPointer<vtkCallbackCommand>::New();
	modeSwitchCallback->SetCallback(modeSwitchFunc);
	modeSwitchCallback->SetClientData(this);
	GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::KeyPressEvent, modeSwitchCallback);
}

//----------------------------------------------------------------------------
std::string vtkPVStuproView::readFile(std::string filename) const
{
	std::string content;
	std::ifstream file(filename);
	if (!file)
	{
		assert(false);
		return content;
	}
	file.seekg(0, std::ios::end);
	content.resize(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(&content[0], content.size());
	return content;
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkOpenGLTexture> vtkPVStuproView::loadAlphaTexture(std::string rgbFile, std::string alphaFile) const
{
	vtkSmartPointer<vtkJPEGReader> imageReader = vtkSmartPointer<vtkJPEGReader>::New();
	imageReader->SetFileName((rgbFile).c_str());

	vtkSmartPointer<vtkJPEGReader> imageReaderHeight = vtkSmartPointer<vtkJPEGReader>::New();
	imageReaderHeight->SetFileName((alphaFile).c_str());

	vtkSmartPointer<vtkImageExtractComponents> extractRedFilter = vtkSmartPointer<vtkImageExtractComponents>::New();
	extractRedFilter->SetInputConnection(imageReader->GetOutputPort());
	extractRedFilter->SetComponents(0);
	extractRedFilter->Update();

	vtkSmartPointer<vtkImageExtractComponents> extractGreenFilter = vtkSmartPointer<vtkImageExtractComponents>::New();
	extractGreenFilter->SetInputConnection(imageReader->GetOutputPort());
	extractGreenFilter->SetComponents(1);
	extractGreenFilter->Update();

	vtkSmartPointer<vtkImageExtractComponents> extractBlueFilter = vtkSmartPointer<vtkImageExtractComponents>::New();
	extractBlueFilter->SetInputConnection(imageReader->GetOutputPort());
	extractBlueFilter->SetComponents(2);
	extractBlueFilter->Update();

	vtkSmartPointer<vtkImageExtractComponents> extractHeightFilter = vtkSmartPointer<vtkImageExtractComponents>::New();
	extractHeightFilter->SetInputConnection(imageReaderHeight->GetOutputPort());
	extractHeightFilter->SetComponents(0);
	extractHeightFilter->Update();


	vtkSmartPointer<vtkImageAppendComponents> appendFilter = vtkSmartPointer<vtkImageAppendComponents>::New();
	appendFilter->SetInputConnection(0, extractRedFilter->GetOutputPort());
	appendFilter->AddInputConnection(0, extractGreenFilter->GetOutputPort());
	appendFilter->AddInputConnection(0, extractBlueFilter->GetOutputPort());
	appendFilter->AddInputConnection(0, extractHeightFilter->GetOutputPort());
	appendFilter->Update();


	vtkSmartPointer<vtkOpenGLTexture> texture = vtkSmartPointer<vtkOpenGLTexture>::New();
	texture->SetInputConnection(appendFilter->GetOutputPort());
	return texture;
}

//----------------------------------------------------------------------------
void vtkPVStuproView::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
