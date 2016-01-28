/*
 * GlobeConfig.hpp
 *
 *  Created on: Jan 22, 2016
 *      Author: bbq
 */

#ifndef SRC_GLOBE_GLOBECONFIG_HPP_
#define SRC_GLOBE_GLOBECONFIG_HPP_

#include <Utils/Math/Vector2.hpp>

/**
 * Holds various configuration values for the globe renderer.
 */
struct GlobeConfig {
	/**
	 * Defines the radius of the visually rendered 3D globe in ParaView world coordinates.
	 */
	float globeRadius;

	/**
	 * Defines the size of the 2D representation of the world map in ParaView world coordinates.
	 */
	Vector2f flatMapSize;

	/**
	 * Defines the size of the internally used plane source for vertex generation.
	 */
	float internalPlaneSize;

	/**
	 * Defines the radius of the earth, used for heightmap calculations.
	 */
	float earthRadius;

	/**
	 * Defines the multiplier for terrain height difference relative to the ocean level.
	 */
	float heightFactor;
};

#endif /* SRC_GLOBE_GLOBECONFIG_HPP_ */
