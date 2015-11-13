#ifndef STUPRO_RECT_HPP
#define STUPRO_RECT_HPP

#include "Vector2.hpp"

template<typename T>
class Rect
{
	public:
		Rect() :
				x(0), y(0), w(0), h(0)
		{
		}

		Rect(T x, T y, T w, T h) :
				x(x), y(y), w(w), h(h)
		{
		}

		Rect(Vector2<T> pos, Vector2<T> size) :
				x(pos.x), y(pos.y), w(size.x), h(size.y)
		{
		}
		
		Rect(const T a[]) :
				x(a[0]), y(a[1]), w(a[2]), h(a[3])
		{
		}

		T * array()
		{
			return &x;
		}

		const T * array() const
		{
			return &x;
		}
		
		T x2() const
		{
			return x + w;
		}
		
		T y2() const
		{
			return y + h;
		}
		
		void x2(T right)
		{
			w = right - x;
		}
		
		void y2(T bottom)
		{
			h = bottom - y;
		}
		
		Vector2<T> x1y1() const
		{
			return Vector2<T>(x, y);
		}
		
		Vector2<T> x1y2() const
		{
			return Vector2<T>(x, y2());
		}
		
		Vector2<T> x2y1() const
		{
			return Vector2<T>(x2(), y);
		}
		
		Vector2<T> x2y2() const
		{
			return Vector2<T>(x2(), y2());
		}
		
		T area() const
		{
			return w * h;
		}
		
		bool invalid() const
		{
			return x != x || y != y || w != w || h != h;
		}

		T x;
		T y;
		T w;
		T h;
		
		static const Rect<T> Zero;
		static const Rect<T> Invalid;
};

template<typename T>
const Rect<T> Rect<T>::Zero = Rect<T>(0, 0, 0, 0);
template<typename T>
const Rect<T> Rect<T>::Invalid = Rect<T>(std::numeric_limits<T>::signaling_NaN(),
                                         std::numeric_limits<T>::signaling_NaN(),
                                         std::numeric_limits<T>::signaling_NaN(),
                                         std::numeric_limits<T>::signaling_NaN());

template<typename T>
Rect<T> operator+(Rect<T> r, Vector2<T> v)
{
	return Rect<T>(r.x + v.x, r.y + v.y, r.w, r.h);
}
template<typename T>
Rect<T> & operator+=(Rect<T> & r, Vector2<T> v)
{
	return r = r + v;
}
template<typename T>
Rect<T> operator-(Rect<T> r, Vector2<T> v)
{
	return Rect<T>(r.x - v.x, r.y - v.y, r.w, r.h);
}
template<typename T>
Rect<T> & operator-=(Rect<T> & r, Vector2<T> v)
{
	return r = r - v;
}
template<typename T>
Rect<T> operator*(Rect<T> r, Vector2<T> v)
{
	return Rect<T>(r.x, r.y, r.w * v.x, r.h * v.y);
}
template<typename T>
Rect<T> & operator*=(Rect<T> & r, Vector2<T> v)
{
	return r = r * v;
}
template<typename T>
Rect<T> operator/(Rect<T> r, Vector2<T> v)
{
	if (v.x == 0 || v.y == 0)
	{
		return Rect<T>::Invalid;
	}
	return Rect<T>(r.x, r.y, r.w / v.x, r.h / v.y);
}
template<typename T>
Rect<T> & operator/=(Rect<T> & r, Vector2<T> v)
{
	return r = r / v;
}

typedef Rect<int> RectI;
typedef Rect<unsigned int> RectU;
typedef Rect<float> RectF;
typedef Rect<double> RectD;

#endif
