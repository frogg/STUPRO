#ifndef STUPRO_VECTOR3_HPP
#define STUPRO_VECTOR3_HPP

#include <Utils/Math/ArithmeticFunctors.hpp>
#include <limits>
#include <cmath>

template<typename T>
class Vector3
{
	public:
		Vector3() :
				x(0), y(0), z(0)
		{
		}

		Vector3(T x, T y, T z) :
				x(x), y(y), z(z)
		{
		}

		Vector3(const T a[]) :
				x(a[0]), y(a[1]), z(a[2])
		{
		}
		
		template <typename T2>
		explicit Vector3(Vector3<T2> v) :
				x(v.x), y(v.y), z(v.z)
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
		
		Vector3<T> cross(Vector3<T> v) const
		{
			return Vector3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
		}
		
		T dot(Vector3<T> v) const
		{
			return x * v.x + y * v.y + z * v.z;
		}
		
		float length() const
		{
			return std::sqrt(dot(*this));
		}
		
		T lengthTyped() const
		{
			return std::sqrt(dot(*this));
		}
		
		bool invalid() const
		{
			return x != x || y != y || z != z;
		}

		T x;
		T y;
		T z;
		
		static const Vector3<T> Zero;
		static const Vector3<T> Invalid;
		
	private:

		template<typename F>
		Vector3<T> genericOperator(Vector3<T> v, F func) const
		{
			v.x = func(x, v.x);
			v.y = func(y, v.y);
			v.z = func(z, v.z);
			return v;
		}

		template<typename F>
		Vector3<T> & genericOperatorAssign(Vector3<T> v, F func)
		{
			x = func(x, v.x);
			y = func(y, v.y);
			z = func(z, v.z);
			return *this;
		}
};

template<typename T>
const Vector3<T> Vector3<T>::Zero = Vector3<T>(0, 0, 0);
template<typename T>
const Vector3<T> Vector3<T>::Invalid = Vector3<T>(std::numeric_limits<T>::signaling_NaN(),
                                                  std::numeric_limits<T>::signaling_NaN(),
                                                  std::numeric_limits<T>::signaling_NaN());

template<typename T>
Vector3<T> operator-(Vector3<T> v)
{
	return Vector3<T>(-v.x, -v.y, -v.z);
}
template<typename T, typename T2>
Vector3<T> operator+(Vector3<T> v, Vector3<T2> v2)
{
	return v.genericOperator(v2, Add<T, T2>());
}
template<typename T, typename T2>
Vector3<T> & operator+=(Vector3<T> & v, Vector3<T2> v2)
{
	return v.genericOperatorAssign(v2, Add<T, T2>());
}
template<typename T, typename T2>
Vector3<T> operator-(Vector3<T> v, Vector3<T2> v2)
{
	return v.genericOperator(v2, Sub<T, T2>());
}
template<typename T, typename T2>
Vector3<T> & operator-=(Vector3<T> & v, Vector3<T2> v2)
{
	return v.genericOperatorAssign(v2, Sub<T, T2>());
}
template<typename T, typename T2>
Vector3<T> operator*(Vector3<T> v, Vector3<T2> v2)
{
	return v.genericOperator(v2, Mul<T, T2>());
}
template<typename T, typename T2>
Vector3<T> & operator*=(Vector3<T> & v, Vector3<T2> v2)
{
	return v.genericOperatorAssign(v2, Mul<T, T2>());
}
template<typename T, typename T2>
Vector3<T> operator/(Vector3<T> v, Vector3<T2> v2)
{
	if (v2.x == 0 || v2.y == 0 || v2.z == 0)
	{
		return Vector3<T>::Invalid;
	}
	return v.genericOperator(v2, Div<T, T2>());
}
template<typename T, typename T2>
Vector3<T> & operator/=(Vector3<T> & v, Vector3<T2> v2)
{
	if (v2.x == 0 || v2.y == 0 || v2.z == 0)
	{
		return v = Vector3<T>::Invalid;
	}
	return v.genericOperatorAssign(v2, Div<T, T2>());
}
template<typename T, typename T2>
Vector3<T> operator*(Vector3<T> v, T2 s)
{
	return Vector3<T>(v.x * s, v.y * s, v.z * s);
}
template<typename T, typename T2>
Vector3<T> & operator*=(Vector3<T> & v, T2 s)
{
	return v = v * s;
}
template<typename T, typename T2>
Vector3<T> operator/(Vector3<T> v, T2 s)
{
	if (s == 0)
	{
		return Vector3<T>::Invalid;
	}
	return Vector3<T>(v.x / s, v.y / s, v.z / s);
}
template<typename T, typename T2>
Vector3<T> & operator/=(Vector3<T> & v, T2 s)
{
	return v = v / s;
}

template<typename T, typename T2>
Vector3<T> operator*(T2 s, Vector3<T> v)
{
	return Vector3<T>(s * v.x, s * v.y, s * v.z);
}
template<typename T, typename T2>
Vector3<T> operator/(T2 s, Vector3<T> v)
{
	if (v.x == 0 || v.y == 0 || v.z == 0)
	{
		return Vector3<T>::Invalid;
	}
	return Vector3<T>(s / v.x, s / v.y, s / v.z);
}

typedef Vector3<int> Vector3i;
typedef Vector3<unsigned int> Vector3u;
typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;

#endif
