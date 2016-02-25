/*
 * HeightmapSampler.hpp
 *
 *  Created on: Feb 23, 2016
 *      Author: bbq
 */

#ifndef SRC_GLOBE_HEIGHTMAPSAMPLER_HPP_
#define SRC_GLOBE_HEIGHTMAPSAMPLER_HPP_

#include <vector>

class HeightmapSampler
{
public:

	static HeightmapSampler & getInstance();

	float sample(float longitude, float latitude) const;

private:

	HeightmapSampler();

	void initHeightmap();

	struct Heightmap
	{
		unsigned int width;
		unsigned int height;
		std::vector<short> samples;
	};
	
	std::vector<Heightmap> heightmaps;
	unsigned int zoomLevel;
	float heightFactor;
};

#endif /* SRC_GLOBE_HEIGHTMAPSAMPLER_HPP_ */
