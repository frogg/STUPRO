#ifndef STUPRO_GLOBETILE_HPP
#define STUPRO_GLOBETILE_HPP

#include "Rect.hpp"
#include "Vector3.hpp"

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkTexture.h>
#include <vtkPlaneSource.h>
#include <vtkShader2.h>

class Globe;

/**
 * Represents a "square" tile as part of the earth globe/map.
 */
class GlobeTile
{
public:
	
	/**
	 * Holds the position and zoom level of a tile.
	 */
	struct Location
	{
		Location(unsigned int zoomLevel, int longitude, int latitude) :
				zoomLevel(zoomLevel),
				longitude(longitude),
				latitude(latitude)
		{
		}
		
		/**
		 * Determines the tile size in terms of earth subdivisions.
		 * 
		 * zoomLevel 0 = 2x1 tiles
		 * zoomLevel 1 = 4x2 tiles
		 * zoomLevel 2 = 8x4 tiles
		 * etc.
		 */
		unsigned int zoomLevel;

		/**
		 * Determines the longitude of the tile. 0 is the column at -180°, and higher numbers describe west tiles.
		 */
		int longitude;

		/**
		 * Determines the latitude of the tile. 0 is the topmost row (-90°), and higher numbers describe south tiles.
		 */
		int latitude;

		/**
		 * Normalizes the location by clamping the tiles to [0;(zoomLevel+1)*2] and [0;zoomLevel+1].
		 */
		Location getNormalized() const;

		/**
		 * Returns the starting/ending longitude/latitude of the tile this location describes.
		 */
		RectF getBounds() const;
	};

	/**
	 * Creates a globe tile belonging to a specific globe at the specified location.
	 */
	GlobeTile(const Globe & manager, Location location);

	/**
	 * Returns the location corresponding to this globe tile.
	 */
	Location getLocation() const;

	/**
	 * Returns the longitude/latitude/width/height of this tile.
	 */
	RectF getBounds() const;
	
	/**
	 * Loads the combined color/heightmap texture from the file corresponding to this tile's
	 * location.
	 */
	void loadTexture();

	/**
	 * Assigns a combined color/heightmap texture to this tile.
	 * 
	 * The RGB channels are interpreted as color information, the alpha channel is interpreted as
	 * height information.
	 */
	void setTexture(vtkSmartPointer<vtkTexture> texture);

	/**
	 * Returns this tile's texture.
	 */
	vtkSmartPointer<vtkTexture> getTexture() const;

	/**
	 * Assigns the height (in meters) corresponding to an alpha value of 0 in this tile's
	 * heightmap.
	 */
	void setLowerHeight(float lower);

	/**
	 * Returns the height (in meters) corresponding to an alpha value of 0 in this tile's
	 * heightmap.
	 */
	float getLowerHeight() const;

	/**
	 * Assigns the height (in meters) corresponding to an alpha value of 255 in this tile's
	 * heightmap.
	 */
	void setUpperHeight(float upper);

	/**
	 * Returns the height (in meters) corresponding to an alpha value of 255 in this tile's
	 * heightmap.
	 */
	float getUpperHeight() const;

	/**
	 * Returns the Actor corresponding to this globe tile.
	 */
	vtkSmartPointer<vtkActor> getActor() const;

private:
	
	/**
	 * Initializes the globe tile's shaders.
	 */
	void initShaders();
	
	const Globe & myGlobe;

	Location myLocation;

	float myLowerHeight;
	float myUpperHeight;

	vtkSmartPointer<vtkActor> myActor;
	vtkSmartPointer<vtkShader2> myVertexShader;
	vtkSmartPointer<vtkShader2> myFragmentShader;
	
};

#endif
