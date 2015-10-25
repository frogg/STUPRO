#ifndef STUPRO_GLOBETILE_HPP
#define STUPRO_GLOBETILE_HPP

#include "Rect.hpp"
#include "Vector3.hpp"

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkTexture.h>
#include <vtkPlaneSource.h>
#include <vtkShader2.h>

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

	GlobeTile(Location location);

	Location getLocation() const;
	RectF getBounds() const;

	void setTexture(vtkSmartPointer<vtkTexture> texture);
	vtkSmartPointer<vtkTexture> getTexture() const;

	void setLowerHeight(float lower);
	float getLowerHeight() const;

	void setUpperHeight(float upper);
	float getUpperHeight() const;

	vtkSmartPointer<vtkActor> getActor() const;

private:

	Location myLocation;

	Vector2u myResolution;

	vtkSmartPointer<vtkActor> myActor;
	vtkSmartPointer<vtkTexture> myTexture;
	vtkSmartPointer<vtkPlaneSource> myPlaneSource;
	vtkSmartPointer<vtkShader2> myVertexShader;
	vtkSmartPointer<vtkShader2> myFragmentShader;

};

#endif
