#include "GlobeTile.hpp"
#include "Utils.hpp"

GlobeTile::Location GlobeTile::Location::getNormalized() const
{
	return Location(zoomLevel, absMod<int>(longitude, zoomLevel * 2), absMod<int>(latitude, zoomLevel));
}

GlobeTile::GlobeTile(Location location)
{
	myLocation = location;
}

RectF GlobeTile::getBounds() const
{
	return myLocation.getBounds();
}

void GlobeTile::setTexture(vtkSmartPointer<vtkTexture> texture)
{
	myTexture = texture;
}

vtkSmartPointer<vtkTexture> GlobeTile::getTexture() const
{
	return myTexture;
}

void GlobeTile::setLowerHeight(float lower)
{
}

float GlobeTile::getLowerHeight() const
{
}

void GlobeTile::setUpperHeight(float upper)
{
}

float GlobeTile::getUpperHeight() const
{
}

vtkSmartPointer<vtkActor> GlobeTile::getActor() const
{
}
