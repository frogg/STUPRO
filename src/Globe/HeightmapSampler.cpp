/*
 * HeightmapSampler.cpp
 *
 *  Created on: Feb 23, 2016
 *      Author: bbq
 */

#include <AsyncWrapper.hpp>
#include <Globe/GlobeTile.hpp>
#include <Globe/HeightmapSampler.hpp>
#include <qimage.h>
#include <qmap.h>
#include <qrgb.h>
#include <Utils/Config/Configuration.hpp>
#include <Utils/Math/Functions.hpp>
#include <Utils/Math/Rect.hpp>
#include <Utils/TileDownload/ImageDownloader.hpp>
#include <Utils/TileDownload/ImageTile.hpp>

HeightmapSampler& HeightmapSampler::getInstance()
{
	static HeightmapSampler * instance = nullptr;

	if (!instance)
	{
		instance = new HeightmapSampler;
	}

	return *instance;
}

float HeightmapSampler::sample(float longitude, float latitude) const
{
	if (longitude < -180 || longitude > 180 || latitude < -90 || longitude > 90)
	{
		return 0;
	}

	unsigned int height = 1 << zoomLevel;
	unsigned int width = height * 2;

	// Find which globe tile the sample falls into.
	int tileX = (longitude + 180) / 360 * width;
	int tileY = (latitude + 90) / 180 * height;

	tileX = clamp<int>(0, tileX, width);
	tileY = clamp<int>(0, tileY, height);

	const Heightmap & heightmap = heightmaps[tileY * width + tileX];

	// Find which pixel the sample falls into.
	RectF tileBounds = GlobeTile::Location(zoomLevel, tileX, tileY).getBounds();

	float normalizedX = (longitude - tileBounds.x) / tileBounds.w;
	float normalizedY = (latitude - tileBounds.y) / tileBounds.h;

	int pixelX = normalizedX * heightmap.width;
	int pixelY = normalizedY * heightmap.height;

	pixelX = clamp<int>(0, pixelX, heightmap.width);
	pixelY = clamp<int>(0, pixelY, heightmap.height);

	short sample = heightmap.samples[pixelY * heightmap.width + pixelX];

	// TODO: make this non-static for when dynamic height factor changing is implemented later
	static float heightFactor = Configuration::getInstance().getFloat("globe.heightFactor");
	return (sample / 6553.f) * heightFactor;
}

HeightmapSampler::HeightmapSampler()
{
	initHeightmap();
}

void HeightmapSampler::initHeightmap()
{
	zoomLevel = Configuration::getInstance().getInteger(
	        "globe.terrainHeightFilter.heightmapZoomLevel");
	unsigned int height = 1 << zoomLevel;
	unsigned int width = height * 2;

	heightmaps.resize(width * height);

	for (unsigned int lat = 0; lat < height; ++lat)
	{
		for (unsigned int lon = 0; lon < width; ++lon)
		{
			Heightmap & heightmap = heightmaps[lat * width + lon];

			AsyncWrapper async;
			ImageTile tile;
			ImageDownloader downloader([&](ImageTile loadedTile)
			{
				tile = loadedTile;
				async.wake();
			});

			downloader.fetchTile(zoomLevel, lon, lat);

			async.sleep();

			auto heightmapIterator = tile.getLayers().find("heightmap");

			if (heightmapIterator == tile.getLayers().end())
			{
				continue;
			}

			const QImage & image = heightmapIterator->getImage();

			unsigned int imageWidth = image.width();
			unsigned int imageHeight = image.height();

			heightmap.width = imageWidth;
			heightmap.height = imageHeight;
			heightmap.samples.resize(imageWidth * imageHeight);

			short minHeight = heightmapIterator->getMinimumHeight();
			short maxHeight = heightmapIterator->getMaximumHeight();

			for (unsigned int y = 0; y < imageHeight; ++y)
			{
				for (unsigned int x = 0; x < imageWidth; ++x)
				{
					float normalizedHeight = qRed(image.pixel(x, y)) / 255.f;
					short height = (1.f - normalizedHeight) * minHeight
					        + (1.f - normalizedHeight) * maxHeight;
					heightmap.samples[y * imageWidth + x] = height;
				}
			}
		}
	}
}
