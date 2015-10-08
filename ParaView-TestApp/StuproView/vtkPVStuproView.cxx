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

	GetRenderer()->AddActor(planeActor);

	vtkSmartPointer<vtkShaderProgram2> pgm = vtkShaderProgram2::New();
	pgm->SetContext(GetRenderWindow());

	//Shader files have to be in the working directory of the compiler or
	//the directory of the executable to be found
	vtkSmartPointer<vtkShader2> fshader = vtkShader2::New();
	fshader->SetType(VTK_SHADER_TYPE_FRAGMENT);
	fshader->SetSourceCode(readFile("TestShader.fsh").c_str());
	fshader->SetContext(pgm->GetContext());

	vtkSmartPointer<vtkShader2> vshader = vtkShader2::New();
	vshader->SetType(VTK_SHADER_TYPE_VERTEX);
	vshader->SetSourceCode(readFile("TestShader.vsh").c_str());
	vshader->SetContext(pgm->GetContext());

	float globeRadius = .5f;
	float planeSize = 1.f;

	vshader->GetUniformVariables()->SetUniformf("globeRadius", 1, &globeRadius);
	vshader->GetUniformVariables()->SetUniformf("planeSize", 1, &planeSize);

	pgm->GetShaders()->AddItem(fshader);
	pgm->GetShaders()->AddItem(vshader);

	vtkSmartPointer<vtkOpenGLProperty> openGLproperty =
		static_cast<vtkOpenGLProperty*>(planeActor->GetProperty());
	openGLproperty->SetPropProgram(pgm);
	openGLproperty->ShadingOn();
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

//----------------------------------------------------------------------------
void vtkPVStuproView::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
