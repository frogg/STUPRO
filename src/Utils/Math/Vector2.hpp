#ifndef STUPRO_VECTOR2_HPP
#define STUPRO_VECTOR2_HPP

#include <Utils/Math/ArithmeticFunctors.hpp>
#include <limits>
#include <cmath>

template<typename T>
class Vector2
{
	public:
		Vector2() :
				x(0), y(0)
		{
		}

		Vector2(T x, T y) :
				x(x), y(y)
		{
		}

		Vector2(const T a[]) :
				x(a[0]), y(a[1])
		{
		}
		
		template <typename T2>
		explicit Vector2(Vector2<T2> v) :
				x(v.x), y(v.y)
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
		
		T dot(Vector2<T> v) const
		{
			return x * v.x + y * v.y;
		}
		
		float length() const
		{
			return std::sqrt(dot(*this));
		}
		
		T lengthTyped() const
		{
			return std::sqrt(dot(*this));
		}
		
		float angle() const
		{
			if (x == 0 && y == 0)
			{
				return 0.f;
			}
			return std::atan2(y, x);
		}
		
		T angleTyped() const
		{
			if (x == 0 && y == 0)
			{
				return 0;
			}
			return std::atan2(y, x);
		}
		
		Vector2<float> norm() const
		{
			return *this / length();
		}
		
		Vector2<T> normTyped() const
		{
			return *this / lengthTyped();
		}
		
		bool invalid() const
		{
			return x != x || y != y;
		}

		T x;
		T y;
		
		static const Vector2<T> Zero;
		static const Vector2<T> Invalid;

	private:

		template<typename F>
		Vector2<T> genericOperator(Vector2<T> v, F func) const
		{
			v.x = func(x, v.x);
			v.y = func(y, v.y);
			return v;
		}

		template<typename F>
		Vector2<T> & genericOperatorAssign(Vector2<T> v, F func)
		{
			x = func(x, v.x);
			y = func(y, v.y);
			return *this;
		}
};

template<typename T>
const Vector2<T> Vector2<T>::Zero = Vector2<T>(0, 0);

template<typename T>
const Vector2<T> Vector2<T>::Invalid = Vector2<T>(std::numeric_limits<T>::signaling_NaN(),
                                                  std::numeric_limits<T>::signaling_NaN());

template<typename T>
Vector2<T> operator-(Vector2<T> v)
{
	return Vector2<T>(-v.x, -v.y);
}
template<typename T, typename T2>
Vector2<T> operator+(Vector2<T> v, Vector2<T2> v2)
{
	return v.genericOperator(v2, Add<T, T2>());
}
template<typename T, typename T2>
Vector2<T> & operator+=(Vector2<T> & v, Vector2<T2> v2)
{
	return v.genericOperatorAssign(v2, Add<T, T2>());
}
template<typename T, typename T2>
Vector2<T> operator-(Vector2<T> v, Vector2<T2> v2)
{
	return v.genericOperator(v2, Sub<T, T2>());
}
template<typename T, typename T2>
Vector2<T> & operator-=(Vector2<T> & v, Vector2<T2> v2)
{
	return v.genericOperatorAssign(v2, Sub<T, T2>());
}
template<typename T, typename T2>
Vector2<T> operator*(Vector2<T> v, Vector2<T2> v2)
{
	return v.genericOperator(v2, Mul<T, T2>());
}
template<typename T, typename T2>
Vector2<T> & operator*=(Vector2<T> & v, Vector2<T2> v2)
{
	return v.genericOperatorAssign(v2, Mul<T, T2>());
}
template<typename T, typename T2>
Vector2<T> operator/(Vector2<T> v, Vector2<T2> v2)
{
	if (v2.x == 0 || v2.y == 0)
	{
		return Vector2<T>::Invalid;
	}
	return v.genericOperator(v2, Div<T, T2>());
}
template<typename T, typename T2>
Vector2<T> & operator/=(Vector2<T> & v, Vector2<T2> v2)
{
	if (v2.x == 0 || v2.y == 0)
	{
		return v = Vector2<T>::Invalid;
	}
	return v.genericOperatorAssign(v2, Div<T, T2>());
}

template<typename T, typename T2>
Vector2<T> operator*(Vector2<T> v, T2 s)
{
	return Vector2<T>(v.x * s, v.y * s);
}
template<typename T, typename T2>
Vector2<T> & operator*=(Vector2<T> & v, T2 s)
{
	return v = v * s;
}
template<typename T, typename T2>
Vector2<T> operator/(Vector2<T> v, T2 s)
{
	if (s == 0)
	{
		return Vector2<T>::Invalid;
	}
	return Vector2<T>(v.x / s, v.y / s);
}
template<typename T, typename T2>
Vector2<T> & operator/=(Vector2<T> & v, T2 s)
{
	return v = v / s;
}

template<typename T, typename T2>
Vector2<T> operator*(T2 s, Vector2<T> v)
{
	return Vector2<T>(s * v.x, s * v.y);
}
template<typename T, typename T2>
Vector2<T> operator/(T2 s, Vector2<T> v)
{
	if (v.x == 0 || v.y == 0)
	{
		return Vector2<T>::Invalid;
	}
	return Vector2<T>(s / v.x, s / v.y);
}

typedef Vector2<int> Vector2i;
typedef Vector2<unsigned int> Vector2u;
typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;

#endif
