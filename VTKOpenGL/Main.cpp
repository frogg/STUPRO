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
#include "vtkJPEGReader.h"
#include "vtkTextureMapToPlane.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"

#include <fstream>
#include <functional>

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

vtkSmartPointer<vtkTexture> getTextureForImageName(std::string filename)
{
    vtkSmartPointer<vtkJPEGReader> jPEGReader = vtkSmartPointer<vtkJPEGReader>::New();
    jPEGReader->SetFileName(("Resources/" + filename).c_str());
    
    vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
    texture->SetInputConnection(jPEGReader->GetOutputPort());
    return texture;
    
}


int main()
{
    vtkSmartPointer<vtkTexture> texture = getTextureForImageName("height.jpg");
    
	vtkSmartPointer<vtkPlaneSource> plane = vtkPlaneSource::New();
	plane->SetResolution(150, 150);

	vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkPolyDataMapper::New();
	planeMapper->SetInputConnection(plane->GetOutputPort());

	vtkSmartPointer<vtkActor> planeActor = vtkActor::New();
	planeActor->SetMapper(planeMapper);
	planeActor->SetTexture(texture);

	vtkSmartPointer<vtkRenderer> ren = vtkRenderer::New();
	ren->AddActor(planeActor);
	
	/*
	auto clipFunc = [ren](vtkObject* caller, unsigned long eventId, void* clientData, void* callData)
	{
		ren->ResetCameraClippingRange(-10, 10, -10, 10, -10, 10);
	};
	vtkSmartPointer<vtkCallbackCommand> clipCallback = vtkSmartPointer<vtkCallbackCommand>::New();
	clipCallback->SetCallback(clipFunc);
	ren->AddObserver(vtkCommand::ResetCameraClippingRangeEvent, clipCallback);
	*/

	vtkSmartPointer<vtkRenderWindow> renWin = vtkRenderWindow::New();
	renWin->SetWindowName("Famous Globe");
	renWin->AddRenderer(ren);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
			vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renWin);

	vtkSmartPointer<vtkInteractorStyle> interactor = vtkInteractorStyleTrackballCamera::New();
	renWin->GetInteractor()->SetInteractorStyle(interactor);

	vtkSmartPointer<vtkShaderProgram2> pgm = vtkShaderProgram2::New();
	pgm->SetContext(renWin);

	vtkSmartPointer<vtkShader2> fshader = vtkShader2::New();
	fshader->SetType(VTK_SHADER_TYPE_FRAGMENT);
	fshader->SetSourceCode(readFile("Shader/TestShader.fsh").c_str());
	fshader->SetContext(pgm->GetContext());

	int textureID = texture->GetTextureUnit();
	fshader->GetUniformVariables()->SetUniformi("texture", 1, &textureID);

	vtkSmartPointer<vtkShader2> vshader = vtkShader2::New();
	vshader->SetType(VTK_SHADER_TYPE_VERTEX);
	vshader->SetSourceCode(readFile("Shader/TestShader.vsh").c_str());
	vshader->SetContext(pgm->GetContext());

	float globeRadius = 0.5f;
	float planeSize = 1.f;
    float heightOffset = 0.2f;
    
    vshader->GetUniformVariables()->SetUniformf("heightOffset", 1, &heightOffset);
	vshader->GetUniformVariables()->SetUniformf("globeRadius", 1, &globeRadius);
	vshader->GetUniformVariables()->SetUniformf("planeSize", 1, &planeSize);
    vshader->GetUniformVariables()->SetUniformi("heightTexture", 1, &textureID);


	pgm->GetShaders()->AddItem(fshader);
	pgm->GetShaders()->AddItem(vshader);

	vtkSmartPointer<vtkOpenGLProperty> openGLproperty = static_cast<vtkOpenGLProperty*>(planeActor->GetProperty());
	openGLproperty->SetPropProgram(pgm);
	openGLproperty->ShadingOn();

	renWin->GetInteractor()->Start();

	/*
	 while (renWin->Is)
	 {
	 //float num = i / 100.f;
	 //vshader->GetUniformVariables()->SetUniformf("bla", 1, &num);
	 renWin->Render();
	 //ren->GetActiveCamera()->Azimuth(1);
	 }
	 */

	return 0;
}

