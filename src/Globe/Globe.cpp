/*
 * Globe.cpp
 *
 *  Created on: Oct 23, 2015
 *      Author: bbq
 */

#include <Globe/Globe.hpp>
#include <qmap.h>
#include <Utils/Config/Configuration.hpp>
#include <Utils/Graphics/TextureLoad.hpp>
#include <Utils/Math/Rect.hpp>
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
#include <cmath>
#include <cstddef>
#include <memory>

Globe::Globe(vtkRenderer& renderer, GlobeConfig globeConfig) :
	myRenderer(renderer),
	myDownloader([ = ](ImageTile tile) {
	onTileLoad(tile);
}),
myTilePool([this]() -> std::unique_ptr<GlobeTile> {
	return makeUnique<GlobeTile>(*this);
}),
myTileLoadCallback([this](void * unused){
	loadGlobeTiles();
}),
myZoomLevel(3),
myDisplayModeInterpolation(0) {
	// TODO: add config option for pool size.
	myTilePool.setPoolSize(32);
	
	QObject::connect(&myTileLoadTimer, SIGNAL(timeout()), &myTileLoadCallback, SLOT(callbackSlot()));
	
	// TODO: add config option for texture load check interval.
	myTileLoadTimer.start(50);

	setGlobeConfig(globeConfig);

	myPlaneSource = vtkPlaneSource::New();

	float planeSize = getGlobeConfig().internalPlaneSize;

	myPlaneSource->SetOrigin(planeSize / 2.f, -planeSize / 2.f, 0.f);
	myPlaneSource->SetPoint1(-planeSize / 2.f, -planeSize / 2.f, 0.f);
	myPlaneSource->SetPoint2(planeSize / 2.f, planeSize / 2.f, 0.f);

	myPlaneMapper = vtkPolyDataMapper::New();
	myPlaneMapper->SetInputConnection(myPlaneSource->GetOutputPort());

	myLoadingTexture = loadTextureFromFile("./res/tiles/TileLoading.png");

	setResolution(Vector2u(64, 64));

	createTileHandles();
}

Globe::~Globe() {
	
	// This is to abort all current tile downloads.
	eraseTileHandles();
}

void Globe::setResolution(Vector2u resolution) {
	myPlaneSource->SetResolution(resolution.x, resolution.y);
}

Vector2u Globe::getResolution() const {
	Vector2i ret;
	myPlaneSource->GetResolution(ret.x, ret.y);
	return Vector2u(ret);
}

vtkSmartPointer<vtkPolyDataMapper> Globe::getPlaneMapper() const {
	return myPlaneMapper;
}

vtkRenderWindow& Globe::getRenderWindow() const {
	return *myRenderer.GetRenderWindow();
}

vtkRenderer& Globe::getRenderer() const {
	return myRenderer;
}

void Globe::setZoomLevel(unsigned int zoomLevel) {
	if (myZoomLevel != zoomLevel) {
		eraseTileHandles();
		myZoomLevel = zoomLevel;
		createTileHandles();
	}
}

unsigned int Globe::getZoomLevel() const {
	return myZoomLevel;
}

GlobeTile& Globe::getTileAt(int lon, int lat) const {
	return getTileHandleAt(lon, lat).getResource();
}

ResourcePool<GlobeTile>::Handle Globe::getTileHandleAt(int lon, int lat) const {
	unsigned int index = getTileIndex(lon, lat);

	assert(index < myTileHandles.size());

	return myTileHandles[index];
}

void Globe::setTileHandleAt(int lon, int lat, ResourcePool<GlobeTile>::Handle handle) {
	unsigned int index = getTileIndex(lon, lat);

	assert(index < myTileHandles.size());

	myTileHandles[index] = handle;
}

unsigned int Globe::getTileIndex(int lon, int lat) const {
	GlobeTile::Location loc = GlobeTile::Location(myZoomLevel, lon, lat).getWrappedLocation();

	unsigned int index = (1 << myZoomLevel) * loc.latitude * 2 + loc.longitude;
	return index;
}

namespace priv {
	/**
	 * Returns the bounding box of the specified vectors.
	 */
	template<typename T, int arraySize>
	Rect<T> getBoundingBox(const std::array<Vector4<T>, arraySize>& vectors) {
		float left, right, top, bottom;

		for (std::size_t i = 0; i < arraySize; ++i) {
			Vector2f pos = vectors[i].xy();

			if (i == 0 || pos.x < left) {
				left = pos.x;
			}

			if (i == 0 || pos.x > right) {
				right = pos.x;
			}

			if (i == 0 || pos.y < top) {
				top = pos.y;
			}

			if (i == 0 || pos.y > bottom) {
				bottom = pos.y;
			}
		}

		return RectF(left, top, right - left, bottom - top);
	}

}

bool Globe::isTileInViewFrustum(int lon, int lat, vtkMatrix4x4* normalTransform,
                                vtkMatrix4x4* compositeTransform) const {
	// Define normalized viewer direction: in screenspace, the camera points in the -Z direction.
	static const Vector3f cameraDirection(0.f, 0.f, -1.f);

	// Define viewport rectangle in screenspace coordinates.
	static const RectF screenRect(-1.f, -1.f, 2.f, 2.f);

	// TODO: Implement culling for flat map!
	if (getDisplayModeInterpolation() > 0.5f) {
		return true;
	}

	// Get location of current tile.
	GlobeTile::Location loc(myZoomLevel, lon, lat);

	// Assume that the current tile is not visible.
	bool visible = false;

	// Calculate surface normals of the current tile's corners.
	std::array<Vector4f, 4> tileCornerNormals;
	tileCornerNormals[0] = Vector4f(loc.getNormalVector(Vector2f(0.f, 0.f)), 0.f);
	tileCornerNormals[1] = Vector4f(loc.getNormalVector(Vector2f(0.f, 1.f)), 0.f);
	tileCornerNormals[2] = Vector4f(loc.getNormalVector(Vector2f(1.f, 0.f)), 0.f);
	tileCornerNormals[3] = Vector4f(loc.getNormalVector(Vector2f(1.f, 1.f)), 0.f);

	// Copy array in case it is needed for position calculations later.
	std::array<Vector4f, 4> tileCornerPositions = tileCornerNormals;

	for (auto& tileNormal : tileCornerNormals) {
		// Transform the surface normal by the normal transformation matrix calculated earlier.
		normalTransform->MultiplyPoint(tileNormal.array(), tileNormal.array());

		// Check if normal points away from the camera.
		if (tileNormal.xyz().dot(cameraDirection) < 0.f) {
			// Other direction? Tile is facing towards viewer, disallow backface culling.
			visible = true;
			break;
		}
	}

	// If tile is facing away from camera, it can not possibly be visible.
	if (!visible) {
		return false;
	}

	// Transform tile position to screenspace.
	for (auto& tilePosition : tileCornerPositions) {

		// Convert normal to position by multiplication with radius.
		tilePosition *= getGlobeConfig().globeRadius;

		// Set homogeneous component to 1 for correct projection calculation.
		tilePosition.w = 1.f;

		// Transform point by view-projection matrix.
		compositeTransform->MultiplyPoint(tilePosition.array(), tilePosition.array());

		// Divide by homogeneous component to complete transformation.
		tilePosition /= tilePosition.w;
	}

	// Calculate the bounding box of the tile's corners.
	RectF boundingBox = priv::getBoundingBox<float, 4>(tileCornerPositions);

	// Intersect the bounding box with the screenspace boundaries.
	visible = boundingBox.intersects(screenRect);

	// Return final visibility state.
	return visible;
}

void Globe::setTileVisibility(int lon, int lat, bool visibility) {
	// Get handle to the globe tile.
	ResourcePool<GlobeTile>::Handle handle = getTileHandleAt(lon, lat);

	// Check if tile is already visible/active and is set to be disabled, or if the tile is set to
	// be enabled from being inactive.
	if (!visibility && handle.isActive()) {
		hideTile(lon, lat);
	} else if (visibility && !handle.isActive()) {
		showTile(lon, lat);
	}
}

void Globe::showTile(int lon, int lat) {
	// Get handle to the globe tile.
	ResourcePool<GlobeTile>::Handle handle = getTileHandleAt(lon, lat);
	
	// Check if the tile resource is already expired.
	if (handle.isExpired()) {
		// A new tile needs to be acquired from the resource pool.
		handle = myTilePool.acquire();

		// Reassign handle.
		setTileHandleAt(lon, lat, handle);

		// Get reference to underlying globe tile.
		GlobeTile& tile = handle.getResource();

		// Make the tile visible.
		tile.setVisibile(true);

		// Move the tile to its new position.
		tile.setLocation(GlobeTile::Location(myZoomLevel, lon, lat));

		// Update the tile's shader uniforms.
		tile.updateUniforms();

		// Give the tile a temporary loading texture.
		tile.setTextureLoadState(GlobeTile::TEXTURE_LOADING);
		tile.setTexture(myLoadingTexture);

		// Start loading process.
		myDownloader.fetchTile(myZoomLevel, lon, lat);
	} else {
		// Re-use existing tile resource by reactivating it.
		handle.setActive(true);

		// Get reference to underlying globe tile.
		GlobeTile& tile = handle.getResource();

		// Make tile visible.
		tile.setVisibile(true);
	}
}

void Globe::hideTile(int lon, int lat) {
	// Get handle to the globe tile.
	ResourcePool<GlobeTile>::Handle handle = getTileHandleAt(lon, lat);

	// Check if handle is currently active.
	if (handle.isActive()) {
		// Get reference to underlying globe tile.
		GlobeTile& tile = handle.getResource();
	
		// Make tile invisible.
		tile.setVisibile(false);
	
		// Mark resource as inactive.
		handle.setActive(false);
	}
}

void Globe::createTileHandles() {
	unsigned int height = 1 << myZoomLevel;
	unsigned int width = height * 2;

	myTileHandles.resize(width * height);
}

void Globe::eraseTileHandles() {
	
	// Cancel all pending downloads.
	myDownloader.abortAllDownloads();
	
	for (auto& handle : myTileHandles) {
		if (handle.isActive()) {
			hideTile(handle.getResource().getLocation().longitude, handle.getResource().getLocation().latitude);
		}

		// Clear handle.
		handle = ResourcePool<GlobeTile>::Handle();
	}
}

void Globe::loadGlobeTiles() {
	std::lock_guard<std::mutex> lock(myDownloadedTilesMutex);

	while (!myDownloadedTiles.empty()) {
		
		ImageTile tile = std::move(myDownloadedTiles.front());
		myDownloadedTiles.pop();
		
		loadGlobeTile(tile);
	}
}

void Globe::loadGlobeTile(const ImageTile & tile) {
	
	if (myZoomLevel != tile.getZoomLevel()) {
		KRONOS_LOG_WARN("Attempt to load zoom-mismatched tile %d,%d", tile.getTileX(), tile.getTileY());
		return;
	}

	ResourcePool<GlobeTile>::Handle handle = getTileHandleAt(tile.getTileX(), tile.getTileY());
	
	bool handleIsActive = handle.isActive();

	if (!handleIsActive) {
		if (handle.isExpired()) {
			KRONOS_LOG_WARN("Attempt to load expired tile %d,%d", tile.getTileX(), tile.getTileY());
			return;
		}
		
		// Temporarily activate globe tile to load texture, deactivate again after loading.
		handle.setActive(true);
	}

	GlobeTile& globeTile = handle.getResource();

	auto rgbIterator = tile.getLayers().find("satelliteImagery");
	auto heightmapIterator = tile.getLayers().find("heightmap");

	if (rgbIterator != tile.getLayers().end() && heightmapIterator != tile.getLayers().end()) {

		const QImage& rgb = rgbIterator->getImage();
		const QImage& heightmap = heightmapIterator->getImage();

		globeTile.loadTexture(rgb, heightmap);
		globeTile.setTextureLoadState(GlobeTile::TEXTURE_LOADED);

		globeTile.setLowerHeight(heightmapIterator->getMinimumHeight());
		globeTile.setUpperHeight(heightmapIterator->getMaximumHeight());

		globeTile.updateUniforms();
		
	} else {
		KRONOS_LOG_WARN("Missing or incomplete image data for tile %d,%d", tile.getTileX(), tile.getTileY());
	}
	
	if (!handleIsActive) {
		// Disable globe tile again.
		handle.setActive(false);
	}
	
}

void Globe::setDisplayModeInterpolation(float displayMode) {
	myDisplayModeInterpolation = displayMode;

	for (const auto& tile : myTileHandles) {
		if (tile.isActive()) {
			tile.getResource().updateUniforms();
		}
	}
}

float Globe::getDisplayModeInterpolation() const {
	return myDisplayModeInterpolation;
}

bool Globe::checkIfRepaintIsNeeded() {
	return !myIsClean.test_and_set();
}

void Globe::onCameraChanged() {
	updateZoomLevel();
	updateTileVisibility();
	loadGlobeTiles();
}

void Globe::setGlobeConfig(GlobeConfig globeConfig) {
	myGlobeConfig = globeConfig;
}

const GlobeConfig& Globe::getGlobeConfig() const {
	return myGlobeConfig;
}

void Globe::onTileLoad(ImageTile tile) {
	std::lock_guard<std::mutex> lock(myDownloadedTilesMutex);

	myDownloadedTiles.push(tile);
}

void Globe::updateZoomLevel() {
	// Get current camera for distance calculations.
	vtkCamera* camera = getRenderer().GetActiveCamera();

	// Get the camera's eye position.
	Vector3d cameraPosition;
	camera->GetPosition(cameraPosition.array());

	// The distance between the camera and the globe's center in globe radius units.
	float cameraDistance = cameraPosition.length() / getGlobeConfig().globeRadius;

	// Get the near and far distance limits from the configuration file.
	float nearDistance = Configuration::getInstance().getFloat("globe.zoom.nearDistance");
	float farDistance = Configuration::getInstance().getFloat("globe.zoom.farDistance");

	// Get the near and far zoom values from the configuration file.
	unsigned int nearZoom = Configuration::getInstance().getInteger("globe.zoom.nearZoom");
	unsigned int farZoom = Configuration::getInstance().getInteger("globe.zoom.farZoom");

	// Normalize the distance between 0.0 and 1.0 for the far and near distances, respectively.
	float normalizedDistance = 1.f - (cameraDistance - nearDistance) / (farDistance - nearDistance);

	// Create variable for resulting zoom level.
	unsigned int zoomResult = 0;

	// Check minimum/maximum cases.
	if (normalizedDistance > 1.f) {
		zoomResult = nearZoom;
	} else if (normalizedDistance < 0.f) {
		zoomResult = farZoom;
	} else {
		// Get the exponential scaling factor.
		float expScale = Configuration::getInstance().getFloat("globe.zoom.expScale");

		// Calculate the exponential normalized distance.
		float exponentialDistance = (std::pow(expScale, normalizedDistance) - 1.f)
		                            / (expScale - 1.f);

		// Interpolate the integer zoom levels based on the exponential normalized distance.
		zoomResult = exponentialDistance * nearZoom + (1.f - exponentialDistance) * farZoom;
	}

	// Assign the zoom level.
	this->setZoomLevel(zoomResult);
}

void Globe::updateTileVisibility() {
	// Get current camera for transformation matrix.
	vtkCamera* camera = getRenderer().GetActiveCamera();

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
	vtkMatrix4x4* fullTransform = camera->GetCompositeProjectionTransformMatrix(
	                                  (float) getRenderer().GetSize()[0] / (float) getRenderer().GetSize()[1], clipNear,
	                                  clipFar);

	// Get number of tiles (vertically and horizontally).
	unsigned int height = 1 << myZoomLevel;
	unsigned int width = height * 2;

	// Iterate over all tiles.
	for (unsigned int lat = 0; lat < height; ++lat) {
		for (unsigned int lon = 0; lon < width; ++lon) {
			// Perform tile visibility check.
			bool visible = isTileInViewFrustum(lon, lat, normalTransform, fullTransform);

			// Change tile visibility.
			setTileVisibility(lon, lat, visible);
		}
	}
}
