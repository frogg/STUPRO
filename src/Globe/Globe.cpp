/*
 * Globe.cpp
 *
 *  Created on: Oct 23, 2015
 *      Author: bbq
 */

#include <Globe/Globe.hpp>
#include <Globe/GlobeTile.hpp>
#include <qmap.h>
#include <Utils/Math/Vector3.hpp>
#include <Utils/Math/Vector4.hpp>
#include <Utils/Misc/MakeUnique.hpp>
#include <Utils/TileDownload/ImageTile.hpp>
#include <vtkAlgorithm.h>
#include <vtkCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <array>
#include <cassert>
#include <cstddef>

Globe::Globe(vtkRenderer & renderer, GlobeConfig globeConfig) :
		myRenderer(renderer),
		myDownloader([=](ImageTile tile)
		{	onTileLoad(tile);}),
		myZoomLevel(3),
		myDisplayModeInterpolation(0)
{
	setGlobeConfig(globeConfig);
	
	myPlaneSource = vtkPlaneSource::New();

	float planeSize = getGlobeConfig().internalPlaneSize;
	
	myPlaneSource->SetOrigin(planeSize / 2.f, -planeSize / 2.f, 0.f);
	myPlaneSource->SetPoint1(-planeSize / 2.f, -planeSize / 2.f, 0.f);
	myPlaneSource->SetPoint2(planeSize / 2.f, planeSize / 2.f, 0.f);
	
	myPlaneMapper = vtkPolyDataMapper::New();
	myPlaneMapper->SetInputConnection(myPlaneSource->GetOutputPort());
	
	setResolution(Vector2u(16, 16));

	createTiles();
}

Globe::~Globe()
{
}

void Globe::setResolution(Vector2u resolution)
{
	myPlaneSource->SetResolution(resolution.x, resolution.y);
}

Vector2u Globe::getResolution() const
{
	Vector2i ret;
	myPlaneSource->GetResolution(ret.x, ret.y);
	return Vector2u(ret);
}

vtkSmartPointer<vtkPolyDataMapper> Globe::getPlaneMapper() const
{
	return myPlaneMapper;
}

vtkRenderWindow& Globe::getRenderWindow() const
{
	return *myRenderer.GetRenderWindow();
}

vtkRenderer& Globe::getRenderer() const
{
	return myRenderer;
}

void Globe::setZoomLevel(unsigned int zoomLevel)
{
	myZoomLevel = zoomLevel;
}

unsigned int Globe::getZoomLevel() const
{
	return myZoomLevel;
}

GlobeTile & Globe::getTileAt(int lon, int lat) const
{
	unsigned int index = getTileIndex(lon, lat);
	
	assert(index < myTiles.size());
	
	return *myTiles[index];
}

unsigned int Globe::getTileIndex(int lon, int lat) const
{
	GlobeTile::Location loc = GlobeTile::Location(myZoomLevel, lon, lat).getWrappedLocation();

	unsigned int index = (1 << myZoomLevel) * loc.latitude * 2 + loc.longitude;
	return index;
}

void Globe::createTiles()
{
	unsigned int height = 1 << myZoomLevel;
	unsigned int width = height * 2;

	myTiles.resize(width * height);

	for (unsigned int lat = 0; lat < height; ++lat)
	{
		for (unsigned int lon = 0; lon < width; ++lon)
		{
			myTiles[getTileIndex(lon, lat)] = makeUnique<GlobeTile>(*this,
			        GlobeTile::Location(myZoomLevel, lon, lat));

			myDownloader.fetchTile(myZoomLevel, lon, lat);
		}
	}
}

void Globe::setDisplayModeInterpolation(float displayMode)
{
	myDisplayModeInterpolation = displayMode;

	for (const auto & tile : myTiles)
	{
		if (tile)
		{
			tile->updateUniforms();
		}
	}
}

float Globe::getDisplayModeInterpolation() const
{
	return myDisplayModeInterpolation;
}

bool Globe::checkIfRepaintIsNeeded()
{
	return !myIsClean.test_and_set();
}

void Globe::updateGlobeTileVisibility()
{
	// Get current camera for transformation matrix.
	vtkCamera * camera = getRenderer().GetActiveCamera();

	// Get normalized viewer direction: in screenspace, the camera points in the -Z direction.
	Vector3f cameraDirection = Vector3f(0.f, 0.f, -1.f);

	// Create a matrix for the normal vector transformation.
	vtkSmartPointer<vtkMatrix4x4> normalTransform = vtkMatrix4x4::New();
	
	// Copy the eye coordinate transformation matrix from the active camera.
	normalTransform->DeepCopy(camera->GetModelViewTransformMatrix());
	
	// Invert & transpose to allow correctly transforming globe tile surface normals.
	normalTransform->Invert();
	normalTransform->Transpose();

	// Assign clipping planes in screenspace (consistency with X/Y coordinate limits).
	double clipNear = -1.f, clipFar = 1.f;

	// Get Model-View-Projection transformation matrix for globe tile position transformation.
	vtkMatrix4x4 * fullTransform = camera->GetCompositeProjectionTransformMatrix(
	        (float) getRenderer().GetSize()[0] / (float) getRenderer().GetSize()[1], clipNear,
	        clipFar);

	// Get number of tiles (vertically and horizontally).
	unsigned int height = 1 << myZoomLevel;
	unsigned int width = height * 2;

	// Iterate over all tiles.
	for (unsigned int lat = 0; lat < height; ++lat)
	{
		for (unsigned int lon = 0; lon < width; ++lon)
		{
			// Begin by making all tiles invisible.
			getTileAt(lon, lat).setVisibility(false);
		}
	}

	// Iterate again over all tiles.
	for (unsigned int lat = 0; lat < height; ++lat)
	{
		for (unsigned int lon = 0; lon < width; ++lon)
		{
			// Get reference to current tile.
			GlobeTile & tile = getTileAt(lon, lat);
			
			// Get location of current tile.
			GlobeTile::Location loc = tile.getLocation();

			// Create array containing all locations of tiles neighboring this tile's top left corner.
			std::array<GlobeTile::Location, 4> neighbors { GlobeTile::Location(loc.zoomLevel,
			        loc.longitude, loc.latitude), GlobeTile::Location(loc.zoomLevel,
			        loc.longitude - 1, loc.latitude), GlobeTile::Location(loc.zoomLevel,
			        loc.longitude, loc.latitude + 1), GlobeTile::Location(loc.zoomLevel,
			        loc.longitude - 1, loc.latitude + 1) };

			// Assume that the current corner is visible.
			bool visible = true;

			// Calculate the surface normal of the current tile's top-left corner.
			Vector4f tileNormal(loc.getNormalVector(Vector2f(0.f, 0.f)), 0.f);
			
			// Calculate the position of the current tile's top-left corner.
			Vector4f tilePosition(tileNormal.xyz() * getGlobeConfig().globeRadius, 1.f);

			// Transform the surface normal by the normal transformation matrix calculated earlier.
			normalTransform->MultiplyPoint(tileNormal.array(), tileNormal.array());

			// Check if normal points in the same direction as the camera.
			if (tileNormal.xyz().dot(cameraDirection) > 0.f)
			{
				// Same direction? Tile is facing away from viewer, perform backface culling.
				visible = false;
			}
			else
			{
				// Transform tile position to screenspace.
				fullTransform->MultiplyPoint(tilePosition.array(), tilePosition.array());

				// Check X, Y and Z coordinates of transformed position.
				for (std::size_t i = 0; i < 3; ++i)
				{
					// Homogenize coordinate by dividing by fourth vector component.
					float & coord = tilePosition.array()[i];
					coord /= tilePosition.w;

					// Check if coordinate is within screenspace bounds.
					if (coord < -1.f || coord > 1.f)
					{
						// Coordinate out of bounds? Tile is visibly out of the viewport, perform
						// frustum culling.
						visible = false;
						break;
					}
				}
			}

			// Was corner not found to be invisible?
			if (visible)
			{
				// Make all neighbors of the current corner visible.
				for (const GlobeTile::Location & neighbor : neighbors)
				{
					getTileAt(neighbor.longitude, neighbor.latitude).setVisibility(true);
				}
			}
		}
	}
}

void Globe::setGlobeConfig(GlobeConfig globeConfig)
{
	myGlobeConfig = globeConfig;
}

const GlobeConfig& Globe::getGlobeConfig() const
{
	return myGlobeConfig;
}

void Globe::onTileLoad(ImageTile tile)
{
	if (myZoomLevel != tile.getZoomLevel())
	{
		return;
	}

	GlobeTile & globeTile = getTileAt(tile.getTileX(), tile.getTileY());

	auto rgbIterator = tile.getLayers().find("satelliteImagery");
	auto heightmapIterator = tile.getLayers().find("heightmap");

	if (rgbIterator == tile.getLayers().end() || heightmapIterator == tile.getLayers().end())
	{
		return;
	}

	const QImage & rgb = rgbIterator->getImage();
	const QImage & heightmap = heightmapIterator->getImage();

	globeTile.setLowerHeight(heightmapIterator->getMinimumHeight());
	globeTile.setUpperHeight(heightmapIterator->getMaximumHeight());

	globeTile.loadTexture(rgb, heightmap);
	globeTile.updateUniforms();

	myIsClean.clear();
}
