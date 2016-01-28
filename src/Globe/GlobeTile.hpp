#ifndef STUPRO_GLOBETILE_HPP
#define STUPRO_GLOBETILE_HPP

#include <Globe/Globe.hpp>
#include <Utils/Math/Rect.hpp>
#include <Utils/Math/Vector2.hpp>
#include <Utils/Math/Vector3.hpp>
#include <vtkActor.h>
#include <vtkShader2.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>

class Globe;

/**
 * Represents a "square" tile as part of the earth globe/map.
 */
class GlobeTile {
public:

	/**
	 * Holds the position and zoom level of a tile.
	 */
	struct Location {
		Location(unsigned int zoomLevel, int longitude, int latitude) :
			zoomLevel(zoomLevel),
			longitude(longitude),
			latitude(latitude) {
		}

		/**
		 * Determines the tile size in terms of earth subdivisions.
		 *
		 * zoomLevel 0 =  2x 1 tiles
		 * zoomLevel 1 =  4x 2 tiles
		 * zoomLevel 2 =  8x 4 tiles
		 * zoomLevel 3 = 16x 8 tiles
		 * zoomLevel 4 = 32x16 tiles
		 * zoomLevel 5 = 64x32 tiles
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
		Location getClampedLocation() const;

		/**
		 * Normalizes the location by wrapping the tiles around [0;(zoomLevel+1)*2] and [0;zoomLevel+1].
		 */
		Location getWrappedLocation() const;

		/**
		 * Returns the starting/ending longitude/latitude of the tile this location describes.
		 */
		RectF getBounds() const;

		/**
		 * Returns the normal vector of this tile when displayed in globe mode.
		 *
		 * @param interpolation
		 *        Bilinear interpolation coefficient for the position within the rectangle:
		 *        0 => left/top, 0.5 => center, 1 => right/bottom
		 */
		Vector3f getNormalVector(Vector2f interpolation = Vector2f(0.5f, 0.5f)) const;
	};

	/**
	 * Creates a globe tile belonging to a specific globe at the specified location.
	 *
	 * Adds the actor to the globe's renderer.
	 */
	GlobeTile(const Globe& manager, Location location);

	/**
	 * Destroys the globe tile and cleans up the actor.
	 */
	~GlobeTile();

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
	void loadTexture(const QImage& rgb, const QImage& height);

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

	/**
	 * Updates the uniform variables of this globe tile's shader.
	 */
	void updateUniforms();

	/**
	 * Sets the visibility of the globe tile.
	 */
	void setVisibility(bool visible);

	/**
	 * Returns the visibility of the globe tile.
	 */
	bool isVisible() const;

private:

	/**
	 * Initializes the globe tile's shaders.
	 */
	void initShaders();

	const Globe& myGlobe;

	Location myLocation;

	float myLowerHeight;
	float myUpperHeight;

	vtkSmartPointer<vtkActor> myActor;
	vtkSmartPointer<vtkShader2> myVertexShader;
	vtkSmartPointer<vtkShader2> myFragmentShader;

	bool myIsVisible;

};

#endif
