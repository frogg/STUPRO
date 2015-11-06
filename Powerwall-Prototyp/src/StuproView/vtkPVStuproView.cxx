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
#include "vtkInteractorStyleTrackballCamera.h"

#include "vtkJPEGReader.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageAppendComponents.h"
#include "vtkTexture.h"
#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyle.h"
#include "StuproInteractor.hpp"
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

	vtkSmartPointer<vtkPlaneSource> plane = vtkPlaneSource::New();
	plane->SetResolution(150, 150);
	
	vtkSmartPointer<vtkTexture> texture = getTextureForImageName("earth.jpg", "height.jpg");

	vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkPolyDataMapper::New();
	planeMapper->SetInputConnection(plane->GetOutputPort());

	vtkSmartPointer<vtkActor> planeActor = vtkActor::New();
	planeActor->SetMapper(planeMapper);
	planeActor->SetTexture(texture);

	GetRenderer()->AddActor(planeActor);


	vtkSmartPointer<vtkShaderProgram2> pgm = vtkShaderProgram2::New();
	pgm->SetContext(GetRenderWindow());

	//Shader files have to be in the working directory of the compiler or
	//the directory of the executable to be found
	vtkSmartPointer<vtkShader2> fshader = vtkShader2::New();
	fshader->SetType(VTK_SHADER_TYPE_FRAGMENT);
	fshader->SetSourceCode(readFile("Shader/TestShader.fsh").c_str());
	fshader->SetContext(pgm->GetContext());

	int textureID = 0; // texture->GetTextureUnit();
	fshader->GetUniformVariables()->SetUniformi("texture", 1, &textureID);

    vshader = vtkShader2::New();
	vshader->SetType(VTK_SHADER_TYPE_VERTEX);
	vshader->SetSourceCode(readFile("Shader/TestShader.vsh").c_str());
	vshader->SetContext(pgm->GetContext());

	float globeRadius = 0.5f;
    float r1 = globeRadius * 100.f;
    float r2 = 0.001;
    float planeSize = 1.f;
    
	float heightOffset = 0.05f;

	vshader->GetUniformVariables()->SetUniformf("interpolation", 1, &interpolationValue);
	vshader->GetUniformVariables()->SetUniformf("heightOffset", 1, &heightOffset);
	vshader->GetUniformVariables()->SetUniformf("globeRadius", 1, &globeRadius);
	vshader->GetUniformVariables()->SetUniformf("planeSize", 1, &planeSize);
	vshader->GetUniformVariables()->SetUniformi("heightTexture", 1, &textureID);
    GetRenderer()->ResetCameraClippingRange(r1, r2, r1, r2, r1, r2);
    // Create interactor for render window.
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<
    vtkRenderWindowInteractor>::New();

    //vtkSmartPointer<vtkInteractorStyle> test =  vtkInteractorStyle::SafeDownCast(GetRenderWindow()->GetInteractor()->GetInteractorStyle());
    
    //test->SetAutoAdjustCameraClippingRange(false);s
    // Create interactor style for render window.
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> interactorStyle = vtkInteractorStyleTrackballCamera::New();
    interactorStyle->SetAutoAdjustCameraClippingRange(false);
    interactor->SetInteractorStyle(interactorStyle);
    //interactor->SetRenderWindow(GetRenderWindow());
	pgm->GetShaders()->AddItem(fshader);
	pgm->GetShaders()->AddItem(vshader);

	vtkSmartPointer<vtkOpenGLProperty> openGLproperty =
		static_cast<vtkOpenGLProperty*>(planeActor->GetProperty());

	openGLproperty->SetPropProgram(pgm);
	openGLproperty->ShadingOn();
    GetRenderWindow()->Render();
}

//----------------------------------------------------------------------------
std::string vtkPVStuproView::readFile(std::string filename)
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

void vtkPVStuproView::switchRepresentation(){
    interpolationValue = interpolationValue == 1.f ? 0.f : 1.f;

    vshader->GetUniformVariables()->SetUniformf("interpolation", 1, &interpolationValue);
    GetRenderer()->GetActiveCamera()->SetPosition(0, 0, 2.8);
    GetRenderWindow()->Render();
    
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkTexture> vtkPVStuproView::getTextureForImageName(std::string picture, std::string heightPicture)
{
	vtkSmartPointer<vtkJPEGReader> imageReader = vtkSmartPointer<vtkJPEGReader>::New();
	imageReader->SetFileName(("Resources/" + picture).c_str());

	vtkSmartPointer<vtkJPEGReader> imageReaderHeight = vtkSmartPointer<vtkJPEGReader>::New();
	imageReaderHeight->SetFileName(("Resources/" + heightPicture).c_str());

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


	vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
	texture->SetInputConnection(appendFilter->GetOutputPort());
	return texture;
}

//----------------------------------------------------------------------------
void vtkPVStuproView::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
