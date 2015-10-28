#include "GlobeTile.hpp"

#include <vtkMapper.h>
#include <vtkOpenGLProperty.h>
#include <vtkProperty.h>
#include <vtkShader2Collection.h>
#include <vtkShaderProgram2.h>
#include <vtkUniformVariables.h>
#include <vtkRenderer.h>
#include <string>

#include "Utils.hpp"
#include "Vector2.hpp"

GlobeTile::Location GlobeTile::Location::getNormalized() const
{
	return Location(zoomLevel, absMod<int>(longitude, (1 << zoomLevel) * 2),
	        absMod<int>(latitude, (1 << zoomLevel)));
}

RectF GlobeTile::Location::getBounds() const
{
	float size = 180.f / (1 << zoomLevel);
	return RectF(longitude * size - 180.f, latitude * size - 90.f, size, size);
}

GlobeTile::GlobeTile(const Globe & globe, Location location) :
		myGlobe(globe),
		myLocation(location)
{
	// Initialize members.
	myLowerHeight = 0.f;
	myUpperHeight = 1.f;
	
	// Create and initialize actor.
	myActor = vtkActor::New();
	
	myActor->SetMapper(myGlobe.getPlaneMapper());
	
	myGlobe.getRenderer().AddActor(myActor);
	
	initShaders();
}

GlobeTile::~GlobeTile()
{
	myGlobe.getRenderer().RemoveActor(myActor);
}

GlobeTile::Location GlobeTile::getLocation() const
{
	return myLocation;
}

RectF GlobeTile::getBounds() const
{
	return myLocation.getBounds();
}

void GlobeTile::setTexture(vtkSmartPointer<vtkTexture> texture)
{
	myActor->SetTexture(texture);
}

vtkSmartPointer<vtkTexture> GlobeTile::getTexture() const
{
	return myActor->GetTexture();
}

void GlobeTile::setLowerHeight(float lower)
{
	myLowerHeight = lower;
}

float GlobeTile::getLowerHeight() const
{
	return myLowerHeight;
}

void GlobeTile::setUpperHeight(float upper)
{
	myUpperHeight = upper;
}

float GlobeTile::getUpperHeight() const
{
	return myLowerHeight;
}

vtkSmartPointer<vtkActor> GlobeTile::getActor() const
{
	return myActor;
}

void GlobeTile::initShaders()
{
	// Create shader program.
	vtkSmartPointer<vtkShaderProgram2> shaderProgram = vtkShaderProgram2::New();
	shaderProgram->SetContext(&myGlobe.getRenderWindow());
	
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
	
	// TODO: Find a way to get texture ID (GetTextureUnit() is missing in ParaView).
	int textureID = 0;
	float globeRadius = 1.f;
	float planeSize = 0.5f;
	float displayModeInterpolation = 0.f;
	float heightFactor = 0.05f;
	
	RectF bounds = myLocation.getBounds();
	Vector2f startBounds = bounds.x1y1();
	Vector2f endBounds = bounds.x2y2();
	
	// Assign uniform variables.
	myVertexShader->GetUniformVariables()->SetUniformi("heightTexture", 1, &textureID);
	
	myVertexShader->GetUniformVariables()->SetUniformf("globeRadius", 1, &globeRadius);
	myVertexShader->GetUniformVariables()->SetUniformf("planeSize", 1, &planeSize);
	myVertexShader->GetUniformVariables()->SetUniformf("displayMode", 1,
	        &displayModeInterpolation);
	myVertexShader->GetUniformVariables()->SetUniformf("heightFactor", 1, &heightFactor);
	
	myVertexShader->GetUniformVariables()->SetUniformf("longStart", 1, &startBounds.x);
	myVertexShader->GetUniformVariables()->SetUniformf("longEnd", 1, &endBounds.x);
	myVertexShader->GetUniformVariables()->SetUniformf("latStart", 1, &startBounds.y);
	myVertexShader->GetUniformVariables()->SetUniformf("latEnd", 1, &endBounds.y);
	
	myFragmentShader->GetUniformVariables()->SetUniformi("texture", 1, &textureID);
	
	// Add shaders to shader program.
	shaderProgram->GetShaders()->AddItem(myFragmentShader);
	shaderProgram->GetShaders()->AddItem(myVertexShader);
	
	// Add shader to globe tile actor.
	vtkSmartPointer<vtkOpenGLProperty> openGLproperty =
	        static_cast<vtkOpenGLProperty*>(myActor->GetProperty());
	openGLproperty->SetPropProgram(shaderProgram);
	openGLproperty->ShadingOn();
}

void GlobeTile::loadTexture()
{
}
