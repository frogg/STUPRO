/*
 * Globe.cpp
 *
 *  Created on: Oct 23, 2015
 *      Author: bbq
 */

#include <Globe/Globe.hpp>
#include <Utils/TileDownload/ImageTile.hpp>
#include <Utils/TileDownload/MetaImage.hpp>
#include <qimage.h>
#include <qmap.h>
#include <vtkAlgorithm.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <View/vtkPVStuproView.h>
#include <Globe/GlobeTile.hpp>
#include <Utils/Misc/MakeUnique.hpp>
#include <Eigen-v3.2.6/Dense>
#include <cmath>

Globe::Globe(vtkRenderer & renderer) :
		myRenderer(renderer), myDownloader([=](ImageTile tile)
		{	onTileLoad(tile);}), myZoomLevel(1), myDisplayModeInterpolation(0)
{

	createOBBTrees();
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

void Globe::createOBBTrees()
{
	myPlaneSource = vtkPlaneSource::New();

	//Vertices of the map
	myPlaneSource->SetOrigin(PLANE_SIZE / 2.f, -PLANE_SIZE / 2.f, 0.f);
	myPlaneSource->SetPoint1(-PLANE_SIZE / 2.f, -PLANE_SIZE / 2.f, 0.f);
	myPlaneSource->SetPoint2(PLANE_SIZE / 2.f, PLANE_SIZE / 2.f, 0.f);

	// create an additional plane and sphere to evaluate the tiles that are visible
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetRadius(GLOBE_RADIUS);
	sphereSource->SetThetaResolution(100);
	sphereSource->SetPhiResolution(100);
	sphereSource->Update();

	// the OBBTree allows us to simulate a single raycast inbetween two given points as seen in the clipFunc
	mySphereTree = vtkSmartPointer<vtkOBBTree>::New();
	mySphereTree->SetDataSet(sphereSource->GetOutput());
	mySphereTree->BuildLocator();

	// an artificial Plane to calculate raycasting coordinates
	vtkSmartPointer<vtkPlaneSource> planeSource = vtkSmartPointer<vtkPlaneSource>::New();
	planeSource->SetOrigin(-PLANE_WIDTH / 2, -PLANE_HEIGHT / 2, 0);
	planeSource->SetPoint1(PLANE_WIDTH / 2, -PLANE_HEIGHT / 2, 0);
	planeSource->SetPoint2(-PLANE_WIDTH / 2, PLANE_HEIGHT / 2, 0);
	planeSource->Update();

	myPlaneTree = vtkSmartPointer<vtkOBBTree>::New();
	myPlaneTree->SetDataSet(planeSource->GetOutput());
	myPlaneTree->BuildLocator();
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

bool Globe::checkIfRepaintIsNeeded()
{
	return !myIsClean.test_and_set();
}

void Globe::updateGlobeTileVisibility()
{
	unsigned int height = 1 << myZoomLevel;
	unsigned int width = height * 2;

	for (unsigned int lat = 0; lat < height; ++lat)
	{
		for (unsigned int lon = 0; lon < width; ++lon)
		{
			std::size_t index = getTileIndex(lon, lat);
			
			if (index >= myTiles.size())
			{
				continue;
			}
			
			myTiles[index]->setVisibility(false);
		}
	}
}

void Globe::onTileLoad(ImageTile tile)
{
	if (myZoomLevel != tile.getZoomLevel())
	{
		return;
	}

	GlobeTile & globeTile = *myTiles[getTileIndex(tile.getTileX(), tile.getTileY())];

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

Vector3d Globe::cutPlanes(double planes[3][4])
{
	Eigen::Matrix3d planeMatrix;
	Eigen::Vector3d offset;
	// fill planeMatrix and offset with content
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			planeMatrix(i, j) = planes[i][j];
		}
		offset(i) = -planes[i][3];
	}

	// solve LGS. colPivHouseholderQr is an algorithm by Eigen (very fast, see doc:
	// http://eigen.tuxfamily.org/dox/group__TopicLinearAlgebraDecompositions.html)
	// if there are still performance problems, call this method not so often
	Eigen::Vector3d cutPoint = planeMatrix.colPivHouseholderQr().solve(offset);

	return Vector3d(cutPoint(0), cutPoint(1), cutPoint(2));
}

Vector3d Globe::getIntersectionPoint(double plane1[4], double plane2[4], double plane3[4],
        Vector3d cameraPosition)
{
	Vector3d intersection;

	double planes[3][4];
	// store them into one array for easier access
	for (int i = 0; i < 4; i++)
	{
		planes[0][i] = plane1[i];
		planes[1][i] = plane2[i];
		planes[2][i] = plane3[i];
	}

    //calculate intersection of planes and store result in intersectionOfPlanes
	Vector3d intersectionOfPlanes = cutPlanes(planes);

	// get intersection with world
	vtkSmartPointer<vtkPoints> intersectPoint = vtkSmartPointer<vtkPoints>::New();
	this->getOBBTree()->IntersectWithLine(cameraPosition.array(), intersectionOfPlanes.array(),
	        intersectPoint, nullptr);

	if (intersectPoint->GetNumberOfPoints() > 0)
	{
		intersectPoint->GetPoint(0, intersection.array());
	}
	else
	{
		// no intersection
		intersection = Vector3d(0, 0, 0);
	}
	return intersection;
}

std::vector<Vector3d> Globe::getIntersectionPoints(double planes[], Vector3d cameraPosition)
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

	// calculate intersection points for each edge of the viewing frustum
	std::vector<Vector3d> worldIntersectionPoints;
	for (int j = 0; j < 4; j++)
	{
		// some math to get the right planes (l/r/l/r and b/b/t/t, the last plane is the far clipping)
		Vector3d intersection = getIntersectionPoint(planeArray[j % 2], planeArray[j / 2 + 2],
		        planeArray[5], cameraPosition);
		worldIntersectionPoints.push_back(intersection);
	}

	return worldIntersectionPoints;
}

std::vector<Coordinate> Globe::getGlobeCoordinates(std::vector<Vector3d> worldPoints)
{
	std::vector<Coordinate> globeCoordinates;
	for (Vector3d worldCoordinate : worldPoints)
	{
		globeCoordinates.push_back(getCoordinatesFromGlobePoint(worldCoordinate.array()));
	}
	return globeCoordinates;
}

std::vector<Coordinate> Globe::getPlaneCoordinates(std::vector<Vector3d> worldPoints)
{
	std::vector<Coordinate> globeCoordinates;
	for (Vector3d worldCoordinate : worldPoints)
	{
		Vector2d point(worldCoordinate.x, worldCoordinate.y);
		globeCoordinates.push_back(getCoordinatesFromPlanePoint(point));
	}
	return globeCoordinates;
}

Coordinate Globe::getCenterGlobeCoordinate(Vector3d cameraPosition)
{
	Vector3d globeOrigin(0, 0, 0);

	vtkSmartPointer<vtkPoints> intersectPoints = vtkSmartPointer<vtkPoints>::New();
	this->getOBBTree()->IntersectWithLine(cameraPosition.array(), globeOrigin.array(),
	        intersectPoints, nullptr);
	Vector3d centerPoint;
	intersectPoints->GetPoint(0, centerPoint.array());
	return getCoordinatesFromGlobePoint(centerPoint.array());
}

vtkSmartPointer<vtkOBBTree> Globe::getOBBTree()
{
	//Map
	if (this->getDisplayModeInterpolation() > 0.9)
	{
		return myPlaneTree;
	}
	else
	{
		//Globe
		return mySphereTree;
	}
}

Coordinate Globe::getCoordinatesFromGlobePoint(Vector3d point)
{
	return Coordinate(asin(point.z / GLOBE_RADIUS) * 180 / M_PI,
	        atan2(point.x, point.y) * 180 / M_PI);
}

Coordinate Globe::getCoordinatesFromPlanePoint(Vector2d point)
{
	double latitude = point.x / (PLANE_WIDTH / 2) * 180;
	double longitude = point.y / (PLANE_HEIGHT / 2) * 90;

	return Coordinate(latitude, longitude);
}
