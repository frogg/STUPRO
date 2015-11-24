#ifndef STUPRO_VECTOR2_HPP
#define STUPRO_VECTOR2_HPP

#include <Utils/Math/ArithmeticFunctors.hpp>
#include <limits>
#include <cmath>

/**
 * Generic 2-dimensional vector class (vector in the mathematical sense).
 */
template<typename T>
class Vector2
{
public:
	
	typedef T ValueType;

	/**
	 * Constructs a zero vector.
	 */
	Vector2() :
			x(0), y(0)
	{
	}

	/**
	 * Constructs a vector from its two coordinates.
	 * 
	 * @param x The x-coordinate for the vector
	 * @param y The y-coordinate for the vector
	 */
	Vector2(T x, T y) :
			x(x), y(y)
	{
	}

	/**
	 * Constructs a vector from an array/pointer.
	 * 
	 * @param a The array to construct the vector from (x = a[0], y = a[1])
	 */
	Vector2(const T a[]) :
			x(a[0]), y(a[1])
	{
	}

	/**
	 * Constructs a vector from another vector of the same component type.
	 * 
	 * @param v The vector to copy the coordinates from.
	 */
	Vector2(const Vector2<T> & v) :
			x(v.x), y(v.y)
	{
	}

	/**
	 * Constructs a vector from another vector of a different component type.
	 * 
	 * @param v The vector to copy the coordinates from, converting them.
	 */
	template<typename T2>
	explicit Vector2(Vector2<T2> v) :
			x(v.x), y(v.y)
	{
	}

	/**
	 * Returns a pointer to the first element of the vector, with the second element directly
	 * following the first.
	 * 
	 * This allows it to be used in VTK function calls.
	 * 
	 * @return A standard-layout pointer to the vector's component array.
	 */
	T * array()
	{
		return &x;
	}

	/**
	 * Returns a const pointer to the first element of the vector, with the second element directly
	 * following the first.
	 * 
	 * This allows it to be used in VTK function calls.
	 * 
	 * @return A constant standard-layout pointer to the vector's component array.
	 */
	const T * array() const
	{
		return &x;
	}

	/**
	 * Returns the dot product (scalar product) of this vector with another one.
	 * 
	 * @param v The vector to dot multiply with.
	 * 
	 * @return The vectors' components multiplied pairwise and summed up.
	 */
	T dot(Vector2<T> v) const
	{
		return x * v.x + y * v.y;
	}

	/**
	 * Returns the length of this vector as a floating point value.
	 * 
	 * @return The square root of the vector's dot product with itself.
	 */
	float length() const
	{
		return std::sqrt(dot(*this));
	}

	/**
	 * Returns the length of this vector as a vector coordinate typed value.
	 * 
	 * @return The square root of the vector's dot product with itself.
	 */
	T lengthTyped() const
	{
		return std::sqrt(dot(*this));
	}

	/**
	 * Returns the angle of the vector as a float in the range [-pi; pi]. If the vector is zero,
	 * returns 0 (as if the vector was pointing right).
	 * 
	 * @return The angle the vector is pointing at
	 */
	float angle() const
	{
		if (x == 0 && y == 0)
		{
			return 0.f;
		}
		return std::atan2(y, x);
	}

	/**
	 * Returns the angle of the vector as a coordinate type in the range [-pi; pi]. If the vector
	 * is zero, returns 0 (as if the vector was pointing right).
	 * 
	 * @return The angle the vector is pointing at
	 */
	T angleTyped() const
	{
		if (x == 0 && y == 0)
		{
			return 0;
		}
		return std::atan2(y, x);
	}

	/**
	 * Returns a normalized float vector pointing in the same direction as this vector, forcing it
	 * to have a length of 1. If the vector is zero, the returned vector will be invalid.
	 * 
	 * @return The vector divided by its length
	 */
	Vector2<float> norm() const
	{
		return Vector2<float>(*this) / length();
	}

	/**
	 * Returns a normalized coordinate typed vector pointing in the same direction as this vector,
	 * forcing it to have a length of 1. If the vector is zero, the returned vector will be invalid.
	 * 
	 * @return The vector divided by its length
	 */
	Vector2<T> normTyped() const
	{
		return *this / lengthTyped();
	}

	/**
	 * Returns whether the vector is an invalid vector (any component is NaN).
	 * 
	 * @return True if the vector is invalid, false otherwise.
	 */
	bool invalid() const
	{
		return x != x || y != y;
	}

	/**
	 * The x component of the vector.
	 */
	T x;

	/**
	 * The y component of the vector.
	 */
	T y;

	/**
	 * A zero vector (0, 0).
	 */
	static const Vector2<T> Zero;

	/**
	 * An invalid vector (NaN, NaN).
	 */
	static const Vector2<T> Invalid;

	/**
	 * Applies a functor with a vector parameter to the vector and returns the result.
	 * 
	 * @param v The vector to combine with this one
	 * @param func The function to apply to this and the passed vector
	 * 
	 * @return The component-wise result of func(*this, v)
	 */
	template<typename F>
	Vector2<T> genericOperator(Vector2<T> v, F func) const
	{
		v.x = func(x, v.x);
		v.y = func(y, v.y);
		return v;
	}

	/**
	 * Applies a functor with a vector parameter to the vector and stores the result in this vector.
	 * 
	 * @param v The vector to combine with this one
	 * @param func The function to apply to this and the passed vector
	 * 
	 * @return A reference to this vector
	 */
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

/**
 * Negates the vector's components and returns the result.
 * 
 * @param v The vector to negate
 * 
 * @return The vector with its components negated
 */
template<typename T>
Vector2<T> operator-(Vector2<T> v)
{
	return Vector2<T>(-v.x, -v.y);
}

/**
 * Adds the vector's components together and returns the result.
 * 
 * @param v The first vector to be added
 * @param v2 The second vector to be added
 * 
 * @return A vector of the summed up components.
 */
template<typename T, typename T2>
Vector2<T> operator+(Vector2<T> v, Vector2<T2> v2)
{
	return v.genericOperator(v2, Add<T, T2>());
}

/**
 * Adds the vector's components together and stores the result.
 * 
 * @param v The vector to be modified
 * @param v2 The vector to be added to the first one
 * 
 * @return A reference to the first vector
 */
template<typename T, typename T2>
Vector2<T> & operator+=(Vector2<T> & v, Vector2<T2> v2)
{
	return v.genericOperatorAssign(v2, Add<T, T2>());
}

/**
 * Subtracts the vector's components and returns the result.
 * 
 * @param v The vector to be subtracted from
 * @param v2 The to subtract
 * 
 * @return A vector of the difference in the components.
 */
template<typename T, typename T2>
Vector2<T> operator-(Vector2<T> v, Vector2<T2> v2)
{
	return v.genericOperator(v2, Subtract<T, T2>());
}

/**
 * Subtracts the vector's components and stores the result.
 * 
 * @param v The vector to be subtracted from
 * @param v2 The to subtract
 * 
 * @return A reference to the first vector.
 */
template<typename T, typename T2>
Vector2<T> & operator-=(Vector2<T> & v, Vector2<T2> v2)
{
	return v.genericOperatorAssign(v2, Subtract<T, T2>());
}

/**
 * Multiplies the vectors' components together and returns the result.
 * 
 * @param v The first vector to be multiplied
 * @param v2 The second vector to be multiplied
 * 
 * @return A vector of the multiplied components.
 */
template<typename T, typename T2>
Vector2<T> operator*(Vector2<T> v, Vector2<T2> v2)
{
	return v.genericOperator(v2, Multiply<T, T2>());
}

/**
 * Multiplies the vectors' components together and stores the result.
 * 
 * @param v The vector to be modified
 * @param v2 The vector to be multiplied with the first one
 * 
 * @return A reference to the first vector.
 */
template<typename T, typename T2>
Vector2<T> & operator*=(Vector2<T> & v, Vector2<T2> v2)
{
	return v.genericOperatorAssign(v2, Multiply<T, T2>());
}

/**
 * Divides the vectors' components and returns the result. If the second vector has null components,
 * the result is an invalid vector.
 * 
 * @param v The vector to be divided
 * @param v2 The vector to be divided by
 * 
 * @return A vector of the divided components.
 */
template<typename T, typename T2>
Vector2<T> operator/(Vector2<T> v, Vector2<T2> v2)
{
	if (v2.x == 0 || v2.y == 0)
	{
		return Vector2<T>::Invalid;
	}
	return v.genericOperator(v2, Divide<T, T2>());
}

/**
 * Divides the vectors' components and stores the result. If the second vector has null components,
 * the result is an invalid vector.
 * 
 * @param v The vector to be modified
 * @param v2 The vector to be divided by
 * 
 * @return A reference to the first vector.
 */
template<typename T, typename T2>
Vector2<T> & operator/=(Vector2<T> & v, Vector2<T2> v2)
{
	if (v2.x == 0 || v2.y == 0)
	{
		return v = Vector2<T>::Invalid;
	}
	return v.genericOperatorAssign(v2, Divide<T, T2>());
}

/**
 * Multiplies the vector's components with a number and returns the result.
 * 
 * @param v The vector to be multiplied
 * @param s The scalar to multiply the vector with
 * 
 * @return The result of the multiplication
 */
template<typename T, typename T2>
Vector2<T> operator*(Vector2<T> v, T2 s)
{
	return Vector2<T>(v.x * s, v.y * s);
}

/**
 * Multiplies the vector's components with a number and stores the result.
 * 
 * @param v The vector to be multiplied
 * @param s The scalar to multiply the vector with
 * 
 * @return A reference to the vector
 */
template<typename T, typename T2>
Vector2<T> & operator*=(Vector2<T> & v, T2 s)
{
	return v = v * s;
}

/**
 * Divides the vector's components by a number and returns the result. If the number is 0, returns an
 * invalid vector.
 * 
 * @param v The vector to be divided
 * @param s The scalar to divide the vector by
 * 
 * @return The result of the division
 */
template<typename T, typename T2>
Vector2<T> operator/(Vector2<T> v, T2 s)
{
	if (s == 0)
	{
		return Vector2<T>::Invalid;
	}
	return Vector2<T>(v.x / s, v.y / s);
}

/**
 * Divides the vector's components by a number and stores the result. If the number is 0, returns an
 * invalid vector.
 * 
 * @param v The vector to be divided
 * @param s The scalar to divide the vector by
 * 
 * @return A reference to the vector
 */
template<typename T, typename T2>
Vector2<T> & operator/=(Vector2<T> & v, T2 s)
{
	return v = v / s;
}

/**
 * Multiplies the vector's components with a number and returns the result.
 * 
 * @param v The vector to be multiplied
 * @param s The scalar to multiply the vector with
 * 
 * @return The result of the multiplication
 */
template<typename T, typename T2>
Vector2<T> operator*(T2 s, Vector2<T> v)
{
	return Vector2<T>(s * v.x, s * v.y);
}

/**
 * Divides the vector's components by a number and returns the result. If the number is 0, returns
 * an invalid vector.
 * 
 * @param v The vector to be divided
 * @param s The scalar to divide the vector by
 * 
 * @return The result of the division
 */
template<typename T, typename T2>
Vector2<T> operator/(T2 s, Vector2<T> v)
{
	if (v.x == 0 || v.y == 0)
	{
		return Vector2<T>::Invalid;
	}
	return Vector2<T>(s / v.x, s / v.y);
}

/**
 * Short-named type aliases for common vector component types.
 */
typedef Vector2<int> Vector2i;
typedef Vector2<unsigned int> Vector2u;
typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;

#endif
