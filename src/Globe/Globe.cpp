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
#include <Utils/Math/Functions.hpp>
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
#include <pqApplicationCore.h>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <memory>

Globe::Globe(vtkRenderer& renderer) :
	myRenderer(renderer),
	myDownloader([ = ](ImageTile tile) {
	onTileLoad(tile);
}),
myTilePool([this]() -> std::unique_ptr<GlobeTile> {
	return makeUnique<GlobeTile>(*this);
}),
myTimerCallback([this](void* unused) {
	updateZoomLevel();
	updateTileVisibility();
	loadGlobeTiles();
	updateDisplayMode(false);
}),
myZoomLevel(0),
myDisplayMode(DisplayGlobe),
myDisplayModeInterpolation(0.f) {
	
	myCachedCameraMatrix.fill(0.0);
	
	myTilePool.setPoolSize(Configuration::getInstance().getInteger("globe.tilePoolSize"));

	// Check if we have a ParaView application instance (and the Event Loop that comes with it).
	if (pqApplicationCore::instance() != nullptr) {

		// Create timer.
		myTimer = makeUnique<QTimer>();

		// Set timer callback.
		QObject::connect(myTimer.get(), SIGNAL(timeout()), &myTimerCallback, SLOT(callbackSlot()));
		
		myTimer->start(Configuration::getInstance().getInteger("globe.timerDelay"));
	}

	myLoadingTexture = loadTextureFromFile("./res/tiles/TileLoading.png");

	generateLODTable();
	createTileHandles();
	updateTileVisibility();
}

Globe::~Globe() {

	// This is to abort all current tile downloads.
	eraseTileHandles();
}

vtkSmartPointer<vtkPolyDataMapper> Globe::getPlaneMapper(float heightDifference) const {

	assert(!myLODTable.empty());

	// Iterate over LOD table.
	for (auto it = myLODTable.begin(); it != myLODTable.end(); ++it) {
		if (heightDifference < it->heightRange) {
			return it->planeMapper;
		}
	}

	return myLODTable.back().planeMapper;
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
	if (getDisplayMode() == DisplayMap) {
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
		tilePosition *= Configuration::getInstance().getFloat("globe.radius");

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

		// Move the tile to its new position.
		tile.setLocation(GlobeTile::Location(myZoomLevel, lon, lat));

		// Give the tile a temporary loading texture (and temporary height settings).
		tile.setTexture(myLoadingTexture);
		tile.setLowerHeight(0.f);
		tile.setUpperHeight(1.f);

		// Update the tile's shader uniforms.
		tile.updateUniforms();

		// Make the tile visible.
		tile.setVisibile(true);

		// Start loading process.
		myDownloader.fetchTile(myZoomLevel, lon, lat);

		KRONOS_LOG_DEBUG("fetching tile %d/%d", lon, lat);
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

	if (!myDownloadedTiles.empty()) {

		while (!myDownloadedTiles.empty()) {

			ImageTile tile = std::move(myDownloadedTiles.front());
			myDownloadedTiles.pop();

			loadGlobeTile(tile);
		}

		getRenderWindow().Render();
	}
}

void Globe::loadGlobeTile(const ImageTile& tile) {

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

		globeTile.setLowerHeight(heightmapIterator->getMinimumHeight());
		globeTile.setUpperHeight(heightmapIterator->getMaximumHeight());

		globeTile.updateUniforms();

	} else {
		KRONOS_LOG_WARN("Missing or incomplete image data for tile %d,%d", tile.getTileX(),
		                tile.getTileY());
	}

	if (!handleIsActive) {
		// Disable globe tile again.
		handle.setActive(false);
	}

}

void Globe::updateDisplayMode(bool instant) {

	float target = (float)myDisplayMode;
	float animSpeed = Configuration::getInstance().getFloat("globe.transitionEffectSpeed");

	if (instant) {
		myDisplayModeInterpolation = target;
	} else if (std::abs(myDisplayModeInterpolation - target) > 0.00001f) {
		myDisplayModeInterpolation = (myDisplayModeInterpolation + target * animSpeed) / (1.f + animSpeed);
	} else {
		// No significant change: no need to update uniforms.
		return;
	}

	for (const auto& tile : myTileHandles) {
		if (tile.isActive()) {
			tile.getResource().updateUniforms();
		}
	}

	getRenderWindow().Render();
}

void Globe::setDisplayMode(DisplayMode displayMode) {
	myDisplayMode = displayMode;

	updateTileVisibility();

	// If the timer is disabled, update display mode instantly.
	if (myTimer == nullptr) {
		updateDisplayMode(true);
	}
}

Globe::DisplayMode Globe::getDisplayMode() const {
	return myDisplayMode;
}

float Globe::getDisplayModeInterpolation() const {
	return myDisplayModeInterpolation;
}

void Globe::onCameraChanged() {
	updateZoomLevel();
	updateTileVisibilityIfNeeded();
	loadGlobeTiles();
}

void Globe::generateLODTable() {
	myLODTable.clear();

	unsigned int minLOD = Configuration::getInstance().getInteger("globe.lod.minimumResolution");
	unsigned int maxLOD = Configuration::getInstance().getInteger("globe.lod.maximumResolution");

	float minDiff = Configuration::getInstance().getFloat("globe.lod.minimumHeightDifference");
	float maxDiff = Configuration::getInstance().getFloat("globe.lod.maximumHeightDifference");

	// Convert LODs to powers of 2.
	minLOD = getNextPowerOf2(minLOD);
	maxLOD = getNextPowerOf2(maxLOD);

	// Iterate over all levels of detail, skipping the highest one.
	for (unsigned int lod = minLOD; lod < maxLOD; lod *= 2) {

		// Perform reverse linear interpolation over level of detail value to get normalized
		// interpolation factor. MaxLOD is divided by 2 as the last LOD is skipped for now, since it
		// is used as a fallback for too high terrain ranges.
		float normalizedLOD = float(lod - minLOD) / float(maxLOD / 2 - minLOD);

		// Calculate the resulting height difference threshold.
		float heightDifference = interpolateLinear(minDiff, maxDiff, normalizedLOD);

		// Add entry to table.
		myLODTable.push_back(LODSetting(heightDifference, lod));
	}

	// Add maximum LOD entry to table.
	myLODTable.push_back(LODSetting(maxDiff, maxLOD));
}

void Globe::onTileLoad(ImageTile tile) {
	std::lock_guard<std::mutex> lock(myDownloadedTilesMutex);

	myDownloadedTiles.push(tile);
}

void Globe::updateZoomLevel() {
	// Get current camera for distance calculations.
	vtkCamera* camera = getRenderer().GetActiveCamera();

	if (camera == nullptr) {
		KRONOS_LOG_WARN("Camera was null while trying to update zoom level.");
		return;
	}

	// Get the camera's eye position.
	Vector3d cameraPosition;
	camera->GetPosition(cameraPosition.array());

	// The distance between the camera and the globe's center in globe radius units.
	float cameraDistance = cameraPosition.length() / Configuration::getInstance().getFloat("globe.radius");

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

void Globe::updateTileVisibilityIfNeeded()
{
	updateTileVisibilityImpl(false);
}

void Globe::updateTileVisibility()
{
	updateTileVisibilityImpl(true);
}

void Globe::updateTileVisibilityImpl(bool forceUpdate) {
	// Get current camera for transformation matrix.
	vtkCamera* camera = getRenderer().GetActiveCamera();

	if (camera == nullptr) {
		KRONOS_LOG_WARN("Camera was null while trying to update tile visibility.");
		return;
	}

	// Assign clipping planes in screenspace (consistency with X/Y coordinate limits).
	double clipNear = -1.f, clipFar = 1.f;

	// Get Model-View-Projection transformation matrix for globe tile position transformation.
	vtkMatrix4x4* fullTransform = camera->GetCompositeProjectionTransformMatrix(
	                                  (float) getRenderer().GetSize()[0] / (float) getRenderer().GetSize()[1], clipNear,
	                                  clipFar);
	
	// Check if matrix is different from last time this function was called.
	bool isMatrixSame = true;
	for (std::size_t i = 0; i < myCachedCameraMatrix.size(); ++i)
	{
		// Compare matrix elements to cached matrix. Copy elements to cache if they are different.
		if (myCachedCameraMatrix[i] != (*(*fullTransform)[i]))
		{
			isMatrixSame = false;
			myCachedCameraMatrix[i] = (*(*fullTransform)[i]);
		}
	}
	
	// Matrix was not changed and force update is false: abort without recomputing tile visibility.
	if (isMatrixSame && !forceUpdate)
	{
		return;
	}

	// Create a matrix for the normal vector transformation.
	vtkSmartPointer<vtkMatrix4x4> normalTransform = vtkMatrix4x4::New();

	// Copy the eye coordinate transformation matrix from the active camera.
	normalTransform->DeepCopy(camera->GetModelViewTransformMatrix());

	// Invert & transpose to allow correctly transforming globe tile surface normals.
	normalTransform->Invert();
	normalTransform->Transpose();

	// Get number of tiles (vertically and horizontally).
	unsigned int height = 1 << myZoomLevel;
	unsigned int width = height * 2;

	// Iterate over all tiles.
	for (unsigned int lat = 0; lat < height; ++lat) {
		for (unsigned int lon = 0; lon < width; ++lon) {
			// Perform tile visibility check.
			bool visible = isTileInViewFrustum(lon, lat, normalTransform, fullTransform);

			// Change tile visibility. Check if visibility actually changed.
			setTileVisibility(lon, lat, visible);
		}
	}
}

Globe::LODSetting::LODSetting(float heightRange, unsigned int lod) :
	heightRange(heightRange), lod(lod) {
	planeSource = vtkPlaneSource::New();

	float planeSize = Configuration::getInstance().getFloat("globe.internalPlaneSize");

	planeSource->SetOrigin(planeSize / 2.f, -planeSize / 2.f, 0.f);
	planeSource->SetPoint1(-planeSize / 2.f, -planeSize / 2.f, 0.f);
	planeSource->SetPoint2(planeSize / 2.f, planeSize / 2.f, 0.f);

	planeSource->SetResolution(lod, lod);

	planeMapper = vtkPolyDataMapper::New();
	planeMapper->SetInputConnection(planeSource->GetOutputPort());
}
