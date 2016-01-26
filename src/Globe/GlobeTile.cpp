#include <Globe/GlobeTile.hpp>
#include <Utils/Graphics/TextureLoad.hpp>
#include <Utils/Math/Functions.hpp>
#include <Utils/Math/Vector2.hpp>
#include <Utils/Math/Vector3.hpp>
#include <Utils/Misc/Macros.hpp>
#include <vtkMapper.h>
#include <vtkOpenGLProperty.h>
#include <vtkProp.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkShader2Collection.h>
#include <vtkShaderProgram2.h>
#include <vtkUniformVariables.h>
#include <cmath>

extern const char* GlobeShader_fsh;
extern const char* GlobeShader_vsh;

GlobeTile::Location GlobeTile::Location::getClampedLocation() const {
	return Location(zoomLevel, std::max<int>(0, std::min<int>(longitude, (1 << zoomLevel) * 2)),
			std::max<int>(0, std::min<int>(latitude, (1 << zoomLevel))));
}

GlobeTile::Location GlobeTile::Location::getWrappedLocation() const {
	return Location(zoomLevel, absoluteModulo<int>(longitude, (1 << zoomLevel) * 2),
			std::max<int>(0, std::min<int>(latitude, (1 << zoomLevel))));
}

RectF GlobeTile::Location::getBounds() const {
	float size = 180.f / (1 << zoomLevel);
	return RectF(longitude * size - 180.f, 90.f - latitude * size - size, size, size);
}

Vector3f GlobeTile::Location::getNormalVector(Vector2f interpolation) const
{
	RectF bounds = getBounds();
	float lon = interpolateLinear(bounds.x, bounds.x2(), interpolation.x);
	float lat = interpolateLinear(bounds.y, bounds.y2(), interpolation.y);

	// Converts a lat/long flat position into a x/y/z globe position.
	lon = (lon + 90.f) * 2.f * KRONOS_PI / 360.f;
	lat = lat * KRONOS_PI / 180.f;

	float cosLat = cos(lat);
	float sinLat = sin(lat);
	float cosLon = cos(lon);
	float sinLon = sin(lon);
	float x = -cosLat * cosLon;
	float y = sinLat;
	float z = cosLat * sinLon;

	return Vector3f(x, y, z);
}

GlobeTile::GlobeTile(const Globe & globe, Location location) :
		myGlobe(globe), myLocation(location), myIsVisible(false)
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

GlobeTile::~GlobeTile() {
	myGlobe.getRenderer().RemoveActor(myActor);
}

GlobeTile::Location GlobeTile::getLocation() const {
	return myLocation;
}

RectF GlobeTile::getBounds() const {
	return myLocation.getBounds();
}

void GlobeTile::setTexture(vtkSmartPointer<vtkTexture> texture) {
	myActor->SetTexture(texture);
}

vtkSmartPointer<vtkTexture> GlobeTile::getTexture() const {
	return myActor->GetTexture();
}

void GlobeTile::setLowerHeight(float lower) {
	myLowerHeight = lower;
}

float GlobeTile::getLowerHeight() const {
	return myLowerHeight;
}

void GlobeTile::setUpperHeight(float upper) {
	myUpperHeight = upper;
}

float GlobeTile::getUpperHeight() const {
	return myLowerHeight;
}

vtkSmartPointer<vtkActor> GlobeTile::getActor() const {
	return myActor;
}

void GlobeTile::initShaders() {
	// Create shader program.
	vtkSmartPointer<vtkShaderProgram2> shaderProgram = vtkShaderProgram2::New();
	shaderProgram->SetContext(&myGlobe.getRenderWindow());

	// Create and load fragment shader.
	myFragmentShader = vtkShader2::New();
	myFragmentShader->SetType(VTK_SHADER_TYPE_FRAGMENT);
	myFragmentShader->SetSourceCode(GlobeShader_fsh);
	myFragmentShader->SetContext(shaderProgram->GetContext());

	// Create and load vertex shader.
	myVertexShader = vtkShader2::New();
	myVertexShader->SetType(VTK_SHADER_TYPE_VERTEX);
	myVertexShader->SetSourceCode(GlobeShader_vsh);
	myVertexShader->SetContext(shaderProgram->GetContext());

	// TODO: Find a way to get texture ID (GetTextureUnit() is missing in ParaView).
	int textureID = 0;
	float globeRadius = myGlobe.getGlobeConfig().globeRadius;
	float planeSize = myGlobe.getGlobeConfig().internalPlaneSize;
	float displayModeInterpolation = 0.f;
	float heightFactor = myGlobe.getGlobeConfig().heightFactor;

	RectF bounds = myLocation.getBounds();
	Vector2f startBounds = bounds.x1y1();
	Vector2f endBounds = bounds.x2y2();

	// Assign uniform variables.
	myVertexShader->GetUniformVariables()->SetUniformi("heightTexture", 1, &textureID);

	myVertexShader->GetUniformVariables()->SetUniformf("globeRadius", 1, &globeRadius);
	myVertexShader->GetUniformVariables()->SetUniformf("planeSize", 1, &planeSize);
	myVertexShader->GetUniformVariables()->SetUniformf("displayMode", 1, &displayModeInterpolation);
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

void GlobeTile::loadTexture(const QImage& rgb, const QImage& height) {
	setTexture(loadAlphaTexture(rgb, height));
}

void GlobeTile::updateUniforms() {
	float displayModeInterpolation = myGlobe.getDisplayModeInterpolation();
	float earthRadius = myGlobe.getGlobeConfig().earthRadius;
	float minHeight = myLowerHeight / earthRadius;
	float maxHeight = myUpperHeight / earthRadius;
	myVertexShader->GetUniformVariables()->SetUniformf("displayMode", 1, &displayModeInterpolation);
	myVertexShader->GetUniformVariables()->SetUniformf("minHeight", 1, &minHeight);
	myVertexShader->GetUniformVariables()->SetUniformf("maxHeight", 1, &maxHeight);
}

void GlobeTile::setVisibility(bool visible) {
	myIsVisible = visible;

	myActor->SetVisibility(visible);
}

bool GlobeTile::isVisible() const {
	return myIsVisible;
}
