#include "GlobeTile.hpp"
#include "Utils.hpp"


#include <vtkShaderProgram2.h>
#include <vtkUniformVariables.h>
#include <vtkShader2Collection.h>
#include <vtkOpenGLProperty.h>

GlobeTile::Location GlobeTile::Location::getNormalized() const
{
	return Location(zoomLevel, absMod<int>(longitude, zoomLevel * 2),
	        absMod<int>(latitude, zoomLevel));
}

RectF GlobeTile::Location::getBounds() const
{
	float size = 180.f / (1 << zoomLevel);
	return RectF(longitude * size - 180.f, latitude * size - 90.f, size, size);
}

GlobeTile::GlobeTile(const Globe & globe, Location location) :
        myGlobe(globe), myLocation(location)
{
	// Initialize members.
	myLowerHeight = 0.f;
	myUpperHeight = 1.f;
	
	// Create and initialize actor.
	myActor = vtkActor::New();
	
	myActor->SetMapper(myGlobe.getPlaneMapper());
	
	initShaders();
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
	myFragmentShader->GetUniformVariables()->SetUniformi("texture", 1, &textureID);
	
	// Assign uniform variables.
	/*
	myVertexShader->GetUniformVariables()->SetUniformf("globeRadius", 1, &myGlobeRadius);
	myVertexShader->GetUniformVariables()->SetUniformf("planeSize", 1, &myPlaneSize);
	myVertexShader->GetUniformVariables()->SetUniformf("displayMode", 1,
	        &myDisplayModeInterpolation);
	myVertexShader->GetUniformVariables()->SetUniformf("heightFactor", 1, &myHeightFactor);
	myVertexShader->GetUniformVariables()->SetUniformi("heightTexture", 1, &textureID);
	*/
	
	// Add shaders to shader program.
	shaderProgram->GetShaders()->AddItem(myFragmentShader);
	shaderProgram->GetShaders()->AddItem(myVertexShader);
	
	// Add shader to globe actor.
	vtkSmartPointer<vtkOpenGLProperty> openGLproperty =
	        static_cast<vtkOpenGLProperty*>(myActor->GetProperty());
	openGLproperty->SetPropProgram(shaderProgram);
	openGLproperty->ShadingOn();
}
