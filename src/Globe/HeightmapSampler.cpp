/*
 * HeightmapSampler.cpp
 *
 *  Created on: Feb 23, 2016
 *      Author: bbq
 */

#include <Utils/Misc/AsyncWrapper.hpp>
#include <Globe/GlobeTile.hpp>
#include <Globe/HeightmapSampler.hpp>
#include <qimage.h>
#include <qmap.h>
#include <qrgb.h>
#include <Utils/Config/Configuration.hpp>
#include <Utils/Math/Functions.hpp>
#include <Utils/Math/Rect.hpp>
#include <Utils/Misc/KronosLogger.hpp>
#include <Utils/TileDownload/ImageDownloader.hpp>
#include <Utils/TileDownload/ImageTile.hpp>

HeightmapSampler& HeightmapSampler::getInstance() {
	static HeightmapSampler* instance = nullptr;

	if (!instance) {
		instance = new HeightmapSampler;
	}

	return *instance;
}

float HeightmapSampler::sample(float longitude, float latitude) const {
	if (longitude < -180 || longitude > 180 || latitude < -90 || latitude > 90) {
		return 0;
	}

	unsigned int height = 1 << zoomLevel;
	unsigned int width = height * 2;

	// Find which globe tile the sample falls into.
	int tileX = ((longitude + 180.f) / 360.f) * width;
	int tileY = ((latitude + 90.f) / 180.f) * height;

	tileX = clamp<int>(0, tileX, width);
	tileY = clamp<int>(0, tileY, height);

	const Heightmap& heightmap = heightmaps[tileY * width + tileX];

	// Find which pixel the sample falls into.
	RectF tileBounds = GlobeTile::Location(zoomLevel, tileX, tileY).getFlippedBounds();

	float normalizedX = (longitude - tileBounds.x) / tileBounds.w;
	float normalizedY = (latitude - tileBounds.y) / tileBounds.h;

	int pixelX = normalizedX * heightmap.width;
	int pixelY = normalizedY * heightmap.height;

	pixelX = clamp<int>(0, pixelX, heightmap.width);
	pixelY = clamp<int>(0, pixelY, heightmap.height);

	short sample = heightmap.samples[pixelY * heightmap.width + pixelX];

	return sample * heightFactor;
}

HeightmapSampler::HeightmapSampler() {
	float earthRadius = Configuration::getInstance().getFloat("globe.earthRadius");
	float globeHeightFactor = Configuration::getInstance().getFloat("globe.heightFactor");
	heightFactor = globeHeightFactor / earthRadius;
	initHeightmap();
}

void HeightmapSampler::initHeightmap() {
	zoomLevel = Configuration::getInstance().getInteger("globe.terrainHeightFilter.heightmapZoomLevel");
	unsigned int height = 1 << zoomLevel;
	unsigned int width = height * 2;

	heightmaps.resize(width * height);

	for (unsigned int lat = 0; lat < height; ++lat) {
		for (unsigned int lon = 0; lon < width; ++lon) {
			Heightmap& heightmap = heightmaps[lat * width + lon];

			bool success = false;
			const char* errorText;
			AsyncWrapper async;
			ImageTile tile;
			ImageDownloader downloader([&](ImageTile loadedTile) {
				// Successfully downloaded.
				success = true;
				tile = loadedTile;
				async.wake();
			}, [&](const std::exception & ex) {
				// Error downloading tile.
				success = false;
				errorText = ex.what();
			});

			downloader.fetchTile(zoomLevel, lon, lat);

			async.sleep();

			if (!success) {
				KRONOS_LOG_WARN("Failed to load tile %u,%u: %s", lon, lat, errorText);
				continue;
			}

			auto heightmapIterator = tile.getLayers().find("heightmap");

			if (heightmapIterator == tile.getLayers().end()) {
				KRONOS_LOG_WARN("Failed to load tile %u,%u: no heightmap data", lon, lat);
				continue;
			}

			const QImage& image = heightmapIterator->getImage();

			unsigned int imageWidth = image.width();
			unsigned int imageHeight = image.height();

			heightmap.width = imageWidth;
			heightmap.height = imageHeight;
			heightmap.samples.resize(imageWidth * imageHeight);

			int minHeight = heightmapIterator->getMinimumHeight();
			int maxHeight = heightmapIterator->getMaximumHeight();

			for (unsigned int y = 0; y < imageHeight; ++y) {
				for (unsigned int x = 0; x < imageWidth; ++x) {
					static const int minShort = -32768;
					static const int maxShort = 32767;

					float normalizedHeight = qRed(image.pixel(x, y)) / 255.f;
					int height = (1.f - normalizedHeight) * minHeight + normalizedHeight * maxHeight;
					height = clamp<int>(minShort, height, maxShort);
					heightmap.samples[y * imageWidth + x] = short(height);
				}
			}
		}
	}
}
