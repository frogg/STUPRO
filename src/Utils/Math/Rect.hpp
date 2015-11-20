#ifndef STUPRO_RECT_HPP
#define STUPRO_RECT_HPP

#include <Utils/Math/Vector2.hpp>
#include <limits>

/**
 * Generic 2D axis aligned rectangle class.
 */
template<typename T>
class Rect
{
public:

	typedef T ValueType;

	/**
	 * Creates a zero-size zero-position rectangle.
	 */
	Rect() :
			x(0), y(0), w(0), h(0)
	{
	}

	/**
	 * Creates a rectangle with the specified width and height components.
	 * 
	 * @param x The x-position of the rectangle
	 * @param y The y-position of the rectangle
	 * @param w The width of the rectangle
	 * @param h The height of the rectangle
	 */
	Rect(T x, T y, T w, T h) :
			x(x), y(y), w(w), h(h)
	{
	}

	/*
	 * Creates a rectangle from a position and a size vector.
	 * 
	 * @param pos The x/y-coordinates of the rectangle
	 * @param size The width/height of the rectangle
	 */
	Rect(Vector2<T> pos, Vector2<T> size) :
			x(pos.x), y(pos.y), w(size.x), h(size.y)
	{
	}

	/**
	 * Creates a rectangle from a packed array in the order x, y, width, height.
	 * 
	 * @param a The array to create the rectangle from.
	 */
	Rect(const T a[]) :
			x(a[0]), y(a[1]), w(a[2]), h(a[3])
	{
	}

	/**
	 * Constructs a rectangle from another rectangle of the same component type.
	 * 
	 * @param r The rectangle to copy the coordinates from.
	 */
	Rect(const Rect<T> & r) :
			x(r.x), y(r.y), w(r.w), h(r.h)
	{
	}

	/**
	 * Constructs a rectangle from another rectangle of a different component type.
	 * 
	 * @param r The rectangle to copy the coordinates from, converting them.
	 */
	template<typename T2>
	explicit Rect(const Rect<T2> & r) :
			x(r.x), y(r.y), w(r.w), h(r.h)
	{
	}

	/**
	 * Returns a pointer to the first component of the rectangle. The array elements are in the
	 * order x, y, width, height.
	 * 
	 * @return A pointer to the rect's components
	 */
	T * array()
	{
		return &x;
	}

	/**
	 * Returns a const pointer to the first component of the rectangle. The array elements are in
	 * the order x, y, width, height.
	 * 
	 * @return A constant pointer to the rect's components
	 */
	const T * array() const
	{
		return &x;
	}

	/**
	 * Returns the x-coordinate of the rectangle's right side.
	 * 
	 * @return x-position + width
	 */
	T x2() const
	{
		return x + w;
	}

	/**
	 * Returns the y-coordinate of the rectangle's bottom side.
	 * 
	 * @return y-position + height
	 */
	T y2() const
	{
		return y + h;
	}

	/**
	 * Resizes the rectangle to the specified width without changing the position.
	 * 
	 * @param right The desired right side x-coordinate for the rectangle
	 */
	void x2(T right)
	{
		w = right - x;
	}

	/**
	 * Resizes the rectangle to the specified height without changing the position.
	 * 
	 * @param bottom The desired bottom side y-coordinate for the rectangle
	 */
	void y2(T bottom)
	{
		h = bottom - y;
	}

	/**
	 * Returns the top left corner of the rectangle.
	 * 
	 * @return The vector (x, y)
	 */
	Vector2<T> x1y1() const
	{
		return Vector2<T>(x, y);
	}

	/**
	 * Returns the bottom left corner of the rectangle.
	 * 
	 * @return The vector (x, y + height)
	 */
	Vector2<T> x1y2() const
	{
		return Vector2<T>(x, y2());
	}

	/**
	 * Returns the top right corner of the rectangle.
	 * 
	 * @return The vector (x + width, y)
	 */
	Vector2<T> x2y1() const
	{
		return Vector2<T>(x2(), y);
	}

	/**
	 * Returns the bottom right corner of the rectangle.
	 * 
	 * @return The vector (x + width, y + height)
	 */
	Vector2<T> x2y2() const
	{
		return Vector2<T>(x2(), y2());
	}

	/**
	 * Returns the area of the rectangle
	 * 
	 * @return width * height
	 */
	T area() const
	{
		return w * h;
	}

	/**
	 * Returns whether this rectangle is invalid (any component is NaN).
	 * 
	 * @return True if the rectangle is invaild, false otherwise.
	 */
	bool invalid() const
	{
		return x != x || y != y || w != w || h != h;
	}

	/**
	 * The x-coordinate of the rectangle's left side.
	 */
	T x;

	/**
	 * The y-coordinate of the rectangle's top side.
	 */
	T y;

	/**
	 * The width of the rectangle.
	 */
	T w;

	/**
	 * The height of the rectangle.
	 */
	T h;

	/**
	 * A zero-position zero-size rectangle.
	 */
	static const Rect<T> Zero;

	/**
	 * An invalid rectangle.
	 */
	static const Rect<T> Invalid;
};

template<typename T>
const Rect<T> Rect<T>::Zero = Rect<T>(0, 0, 0, 0);
template<typename T>
const Rect<T> Rect<T>::Invalid = Rect<T>(std::numeric_limits<T>::signaling_NaN(),
        std::numeric_limits<T>::signaling_NaN(), std::numeric_limits<T>::signaling_NaN(),
        std::numeric_limits<T>::signaling_NaN());

/**
 * Moves the rectangle by the specified offset and returns the result.
 * 
 * @param r The rectangle to move
 * @param v The offset to move the rectangle by
 * 
 * @return The rectangle moved by the vector
 */
template<typename T>
Rect<T> operator+(Rect<T> r, Vector2<T> v)
{
	return Rect<T>(r.x + v.x, r.y + v.y, r.w, r.h);
}

/**
 * Moves the rectangle by the specified offset and stores the result.
 * 
 * @param r The rectangle to move
 * @param v The offset to move the rectangle by
 * 
 * @return A reference to the rectangle
 */
template<typename T>
Rect<T> & operator+=(Rect<T> & r, Vector2<T> v)
{
	return r = r + v;
}

/**
 * Moves the rectangle negatively by the specified offset and returns the result.
 * 
 * @param r The rectangle to move
 * @param v The negated offset to move the rectangle by
 * 
 * @return The rectangle moved by the negated vector
 */
template<typename T>
Rect<T> operator-(Rect<T> r, Vector2<T> v)
{
	return Rect<T>(r.x - v.x, r.y - v.y, r.w, r.h);
}

/**
 * Moves the rectangle negatively by the specified offset and stores the result.
 * 
 * @param r The rectangle to move
 * @param v The negated offset to move the rectangle by
 * 
 * @return A reference to the rectangle
 */
template<typename T>
Rect<T> & operator-=(Rect<T> & r, Vector2<T> v)
{
	return r = r - v;
}

/**
 * Scales the rectangle's size by the specified factor without moving it and returns the result.
 * 
 * @param r The rectangle to scale
 * @param v The factor to scale the rectangle by
 * 
 * @return The rectangle scaled by the vector
 */
template<typename T>
Rect<T> operator*(Rect<T> r, Vector2<T> v)
{
	return Rect<T>(r.x, r.y, r.w * v.x, r.h * v.y);
}

/**
 * Scales the rectangle's size by the specified factor without moving it and stores the result.
 * 
 * @param r The rectangle to scale
 * @param v The factor to scale the rectangle by
 * 
 * @return A reference to the rectangle
 */
template<typename T>
Rect<T> & operator*=(Rect<T> & r, Vector2<T> v)
{
	return r = r * v;
}

/**
 * Scales the rectangle's size by the inverse of the specified factor without moving it and returns
 * the result.
 * 
 * @param r The rectangle to scale
 * @param v The inverted factor to scale the rectangle by
 * 
 * @return The rectangle scaled by the inverted vector
 */
template<typename T>
Rect<T> operator/(Rect<T> r, Vector2<T> v)
{
	if (v.x == 0 || v.y == 0)
	{
		return Rect<T>::Invalid;
	}
	return Rect<T>(r.x, r.y, r.w / v.x, r.h / v.y);
}

/**
 * Scales the rectangle's size by the inverse of the specified factor without moving it and stores
 * the result.
 * 
 * @param r The rectangle to scale
 * @param v The inverted factor to scale the rectangle by
 * 
 * @return A reference to the rectangle
 */
template<typename T>
Rect<T> & operator/=(Rect<T> & r, Vector2<T> v)
{
	return r = r / v;
}

/**
 * Short-named type aliases for common vector component types.
 */
typedef Rect<int> RectI;
typedef Rect<unsigned int> RectU;
typedef Rect<float> RectF;
typedef Rect<double> RectD;

#endif
