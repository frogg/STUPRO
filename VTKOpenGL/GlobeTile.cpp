#include "GlobeTile.hpp"
#include "Utils.hpp"

GlobeTile::Location GlobeTile::Location::getNormalized() const
{
	return Location(zoomLevel, absMod<int>(longitude, zoomLevel * 2), absMod<int>(latitude, zoomLevel));
}

RectF GlobeTile::Location::getBounds() const
{
	float size = 180.f / (1 << zoomLevel);
	return RectF(longitude * size - 180.f, latitude * size - 90.f, size, size);
}

GlobeTile::GlobeTile(const GlobeManager & manager, Location location) :
		myManager(manager)
{
	myLocation = location;
	myLowerHeight = 0.f;
	myUpperHeight = 1.f;
	myActor = vtkActor::New();
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
