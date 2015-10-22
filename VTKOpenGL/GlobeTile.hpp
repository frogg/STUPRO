#ifndef STUPRO_GLOBETILE_HPP
#define STUPRO_GLOBETILE_HPP

#include "Rect.hpp"
#include "Vector3.hpp"

class GlobeTile
{
public:
	
	GlobeTile();
	
	void setBounds(RectF bounds);
	RectF getBounds() const;
	
	void setResolution(Vector2u resolution);
	Vector2u getResolution() const;
	
private:
	
	RectF myBounds;
	
	Vector2u myResolution;
};

#endif
