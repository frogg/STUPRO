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
#include "vtkObject.h"

#include <iostream>
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

int main()
{
	vtkSmartPointer<vtkJPEGReader> jPEGReader = vtkSmartPointer<vtkJPEGReader>::New();
	jPEGReader->SetFileName("Ressources/earth.jpg");

	vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
	texture->SetInputConnection(jPEGReader->GetOutputPort());

	vtkSmartPointer<vtkPlaneSource> plane = vtkPlaneSource::New();
	plane->SetResolution(100, 100);

	vtkSmartPointer<vtkTextureMapToPlane> texturePlane = vtkSmartPointer<vtkTextureMapToPlane>::New();
	texturePlane->SetInputConnection(plane->GetOutputPort());

	vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkPolyDataMapper::New();
	planeMapper->SetInputConnection(plane->GetOutputPort());

	vtkSmartPointer<vtkActor> planeActor = vtkActor::New();
	planeActor->SetMapper(planeMapper);
	planeActor->SetTexture(texture);

	vtkSmartPointer<vtkRenderer> ren = vtkRenderer::New();
	ren->AddActor(planeActor);
	
	auto clipFunc = [](vtkObject* caller, unsigned long eventId, void* clientData, void* callData)
	{
		float range = 2.f;
		((vtkRenderer*)caller)->ResetCameraClippingRange(-range, range, -range, range, -range, range);
	};
	vtkSmartPointer<vtkCallbackCommand> clipCallback = vtkSmartPointer<vtkCallbackCommand>::New();
	clipCallback->SetCallback(clipFunc);
	ren->AddObserver(vtkCommand::ResetCameraClippingRangeEvent, clipCallback);
	clipFunc(ren, 0, 0, 0);

	vtkSmartPointer<vtkRenderWindow> renWin = vtkRenderWindow::New();
	renWin->SetWindowName("VTK Shader Test");
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
	float interpolation = 0.f;

	vshader->GetUniformVariables()->SetUniformf("globeRadius", 1, &globeRadius);
	vshader->GetUniformVariables()->SetUniformf("planeSize", 1, &planeSize);
	vshader->GetUniformVariables()->SetUniformf("interpolation", 1, &interpolation);

	pgm->GetShaders()->AddItem(fshader);
	pgm->GetShaders()->AddItem(vshader);

	vtkSmartPointer<vtkOpenGLProperty> openGLproperty = static_cast<vtkOpenGLProperty*>(planeActor->GetProperty());
	openGLproperty->SetPropProgram(pgm);
	openGLproperty->ShadingOn();
	
	struct InterpolateClient
	{
		vtkUniformVariables * vars;
		float * interpolation;
		float amount;
		vtkRenderWindow * ren;
	};
	
	auto interpolateFunc = [](vtkObject* caller, unsigned long eventId, void* clientData, void* callData)
	{
		InterpolateClient & cld = *(InterpolateClient*)clientData;
		vtkRenderWindowInteractor * interactor = (vtkRenderWindowInteractor*)caller;
		if (interactor->GetKeyCode() == 49) {
			*cld.interpolation -= cld.amount;
		} else if (interactor->GetKeyCode() == 50) {
			*cld.interpolation += cld.amount;
		}
		if (*cld.interpolation > 1.0)
			*cld.interpolation = 1.0;
		if (*cld.interpolation < 0.0)
			*cld.interpolation = 0.0;
		cld.vars->SetUniformf("interpolation", 1, cld.interpolation);
		cld.ren->Render();
	};
	InterpolateClient interpolateClient = {vshader->GetUniformVariables(), &interpolation, 0.01, renWin};
	vtkSmartPointer<vtkCallbackCommand> interpolateCallback = vtkSmartPointer<vtkCallbackCommand>::New();
	interpolateCallback->SetCallback(interpolateFunc);
	interpolateCallback->SetClientData(&interpolateClient);
	renWin->GetInteractor()->AddObserver(vtkCommand::KeyPressEvent, interpolateCallback);

	renWin->GetInteractor()->Start();

	return 0;
}

