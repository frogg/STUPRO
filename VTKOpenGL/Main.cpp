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

#include <fstream>

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
	float lowLim = -0.5;
	float uppLim = 0.5;
	
	vtkSmartPointer<vtkPlaneSource> plane = vtkPlaneSource::New();
	plane->SetResolution(100, 100);
	plane->SetPoint1(uppLim, lowLim, 0);
	plane->SetPoint2(lowLim, uppLim, 0);

	vtkSmartPointer<vtkPolyDataMapper> planeMapper = vtkPolyDataMapper::New();
	planeMapper->SetInputConnection(plane->GetOutputPort());

	vtkSmartPointer<vtkActor> planeActor = vtkActor::New();
	planeActor->SetMapper(planeMapper);

	vtkSmartPointer<vtkRenderer> ren = vtkRenderer::New();
	ren->AddActor(planeActor);

	vtkSmartPointer<vtkRenderWindow> renWin = vtkRenderWindow::New();
	renWin->SetWindowName("VTK Shader Test");
	renWin->AddRenderer(ren);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
			vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renWin);

	vtkSmartPointer<vtkInteractorStyle> interactor =
			vtkInteractorStyleTrackballCamera::New();
	renWin->GetInteractor()->SetInteractorStyle(interactor);

	vtkSmartPointer<vtkShaderProgram2> pgm = vtkShaderProgram2::New();
	pgm->SetContext(renWin);

	vtkSmartPointer<vtkShader2> fshader = vtkShader2::New();
	fshader->SetType(VTK_SHADER_TYPE_FRAGMENT);
	fshader->SetSourceCode(readFile("TestShader.fsh").c_str());
	fshader->SetContext(pgm->GetContext());

	vtkSmartPointer<vtkShader2> vshader = vtkShader2::New();
	vshader->SetType(VTK_SHADER_TYPE_VERTEX);
	vshader->SetSourceCode(readFile("TestShader.vsh").c_str());
	vshader->SetContext(pgm->GetContext());
	
	float globeRadius = 0.5f;
	float planeSize = 1.f;
	
	vshader->GetUniformVariables()->SetUniformf("globeRadius", 1, &globeRadius);
	vshader->GetUniformVariables()->SetUniformf("planeSize", 1, &planeSize);

	pgm->GetShaders()->AddItem(fshader);
	pgm->GetShaders()->AddItem(vshader);

	vtkSmartPointer<vtkOpenGLProperty> openGLproperty =
			static_cast<vtkOpenGLProperty*>(planeActor->GetProperty());
	openGLproperty->SetPropProgram(pgm);
	openGLproperty->ShadingOn();

	renWin->GetInteractor()->Start();

	return 0;
}

