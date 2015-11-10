/*
 * Globe.cpp
 *
 *  Created on: Oct 23, 2015
 *      Author: bbq
 */

#include "Globe.hpp"

#include "ImageTile.hpp"
#include "MetaImage.hpp"
#include <qimage.h>
#include <qmap.h>
#include <vtkAlgorithm.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include "GlobeTile.hpp"
#include "MakeUnique.hpp"

#include "Eigen-v3.2.6/Dense"

Globe::Globe(vtkRenderer & renderer) :
		myRenderer(renderer),
		myDownloader([=](ImageTile tile)
		{	onTileLoad(tile);}),
		myZoomLevel(1),
		myDisplayModeInterpolation(0)
{
	myPlaneSource = vtkPlaneSource::New();
	myPlaneSource->SetOrigin(getPlaneSize() / 2.f, -getPlaneSize() / 2.f, 0.f);
	myPlaneSource->SetPoint1(-getPlaneSize() / 2.f, -getPlaneSize() / 2.f, 0.f);
	myPlaneSource->SetPoint2(getPlaneSize() / 2.f, getPlaneSize() / 2.f, 0.f);

	setResolution(Vector2u(128, 128));

	myPlaneMapper = vtkPolyDataMapper::New();
	myPlaneMapper->SetInputConnection(myPlaneSource->GetOutputPort());

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
	return *myTiles[getTileIndex(lon, lat)];
}

unsigned int Globe::getTileIndex(int lon, int lat) const
{
	GlobeTile::Location loc = GlobeTile::Location(myZoomLevel, lon, lat).getNormalized();

	return (1 << myZoomLevel) * loc.latitude * 2 + loc.longitude;
}

float Globe::getPlaneSize() const
{
	return 1.f;
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
			myTiles[getTileIndex(lon, lat)] = makeUnique<GlobeTile>(*this, GlobeTile::Location(myZoomLevel, lon, lat));

			myDownloader.getTile(myZoomLevel, lon, lat);
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

bool Globe::checkDirty()
{
	return !myIsClean.test_and_set();
}

void Globe::onTileLoad(ImageTile tile)
{
	if (myZoomLevel != tile.getZoomLevel())
	{
		return;
	}

	GlobeTile & globeTile = *myTiles[getTileIndex(tile.getTileX(), tile.getTileY())];

	auto rgbIterator = tile.getLayers().find("satellite-imagery");
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

void Globe::cutPlanes(double planes[3][4], double cut[3])
{
	Eigen::Matrix3d planeMatrix;
	Eigen::Vector3d offset;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			planeMatrix(i, j) = planes[i][j];
		}
		offset(i) = -planes[i][3];
	}

	Eigen::Vector3d cutPoint = planeMatrix.colPivHouseholderQr().solve(offset); //.lu().solve(offset);

	// copy return value to avoid memory issues
	for (int i = 0; i < 3; i++)
	{
		cut[i] = cutPoint(i);
	}
}

void Globe::getIntersectionPoint(double plane1[4], double plane2[4], double plane3[4], double cameraPosition[3],
		vtkSmartPointer<vtkOBBTree> tree, double intersection[3])
{

	double planes[3][4];
	double intersectionOfPlanes[3];
	for (int i = 0; i < 4; i++)
	{
		planes[0][i] = plane1[i];
		planes[1][i] = plane2[i];
		planes[2][i] = plane3[i];
	}

	cutPlanes(planes, intersectionOfPlanes);

	vtkSmartPointer<vtkPoints> intersectPoint = vtkSmartPointer<vtkPoints>::New();
	tree->IntersectWithLine(cameraPosition, intersectionOfPlanes, intersectPoint, NULL);

	if (intersectPoint->GetNumberOfPoints() > 0)
	{
		intersectPoint->GetPoint(0, intersection);
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			intersection[i] = 0;
		}
	}
}

std::vector<Vector3d> Globe::getIntersectionPoints(double planes[], double cameraPosition[],
		vtkSmartPointer<vtkOBBTree> tree)
{
	// left, right, bottom, top, near, far
	double planeArray[6][4];
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			planeArray[i][j] = planes[4 * i + j];
		}
	}

	std::vector<Vector3d> worldIntersectionPoints;
	for (int j = 0; j < 4; j++)
	{
		Vector3d intersection;

		getIntersectionPoint(planeArray[j % 2], planeArray[j / 2 + 2], planeArray[5], cameraPosition, tree,
			intersection.array());
		worldIntersectionPoints.push_back(intersection);
	}

	return worldIntersectionPoints;
}

std::vector<Coordinate> Globe::getGlobeCoordinates(std::vector<Vector3d> worldPoints, double radius)
{
	std::vector<Coordinate> globeCoordinates;
	for (Vector3d worldCoordinate : worldPoints)
	{
		globeCoordinates.push_back(Coordinate::getCoordinatesFromGlobePoint(worldCoordinate.array(), radius));
	}
	return globeCoordinates;
}

std::vector<Coordinate> Globe::getPlaneCoordinates(std::vector<Vector3d> worldPoints, double planeWidth,
		double planeHeight)
{
	std::vector<Coordinate> globeCoordinates;
	for (Vector3d worldCoordinate : worldPoints)
	{
		globeCoordinates.push_back(
			Coordinate::getCoordinatesFromPlanePoint(worldCoordinate.x, worldCoordinate.y, planeWidth, planeHeight));
	}
	return globeCoordinates;
}

Coordinate Globe::getCenterGlobeCoordinate(vtkSmartPointer<vtkOBBTree> tree, double cameraPosition[],
		double globeRadius)
{
	double globeOrigin[3] =
	{ 0, 0, 0 };

	vtkSmartPointer<vtkPoints> intersectPoints = vtkSmartPointer<vtkPoints>::New();
	tree->IntersectWithLine(cameraPosition, globeOrigin, intersectPoints, NULL);
	Vector3d centerPoint;
	intersectPoints->GetPoint(0, centerPoint.array());
	return Coordinate::getCoordinatesFromGlobePoint(centerPoint.array(), globeRadius);
}
