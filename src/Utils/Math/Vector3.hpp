#ifndef STUPRO_VECTOR3_HPP
#define STUPRO_VECTOR3_HPP

#include <Utils/Math/Vector2.hpp>
#include <Utils/Math/ArithmeticFunctors.hpp>
#include <limits>
#include <cmath>
#include <QString>

#ifndef _MSC_VER
template<typename T> class Spherical;
template<typename T> class Cartesian;
#endif

/**
 * Generic 3-dimensional vector class (vector in the mathematical sense).
 */
template<typename T>
class Vector3 {
public:

	typedef T ValueType;

	/**
	 * Constructs a zero vector.
	 */
	Vector3() :
		x(0), y(0), z(0) {
	}

	/**
	 * Constructs a vector from its three coordinates.
	 *
	 * @param x The x-coordinate for the vector
	 * @param y The y-coordinate for the vector
	 * @param z The z-coordinate for the vector
	 */
	Vector3(T x, T y, T z) :
		x(x), y(y), z(z) {
	}

	/**
	 * Constructs a vector from an array/pointer.
	 *
	 * @param a The array to construct the vector from (x = a[0], y = a[1], y = a[2])
	 */
	Vector3(const T a[]) :
		x(a[0]), y(a[1]), z(a[2]) {
	}

	/**
	 * Constructs a vector from another vector of the same component type.
	 *
	 * @param v The vector to copy the coordinates from.
	 */
	Vector3(const Vector3<T>& v) :
		x(v.x), y(v.y), z(v.z) {
	}

	/**
	 * Constructs a 3D vector from a 2D vector and a z-coordinate.
	 *
	 * @param xy The x and y coordinates for the vector
	 * @param z The z-coordinate for the vector
	 */
	Vector3(Vector2<T> xy, T z) :
		x(xy.x), y(xy.y), z(z) {
	}

	/**
	 * Constructs a vector from another vector of a different component type.
	 *
	 * @param v The vector to copy the coordinates from, converting them.
	 */
	template<typename T2>
	explicit Vector3(Vector3<T2> v) :
		x(v.x), y(v.y), z(v.z) {
	}

	/**
	 * Returns a pointer to the first element of the vector, with the second element directly
	 * following the first.
	 *
	 * This allows it to be used in VTK function calls.
	 *
	 * @return A standard-layout pointer to the vector's component array.
	 */
	T* array() {
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
	const T* array() const {
		return &x;
	}

	/**
	 * @return A 2D vector with the x and y components of this vector.
	 */
	Vector2<T> xy() const {
		return Vector2<T>(x, y);
	}

	/**
	 * Returns the cross product (vector product) of this vector with another one.
	 *
	 * @param v The vector to cross multiply with.
	 *
	 * @return The vectors' components cross-multiplied.
	 */
	Vector3<T> cross(Vector3<T> v) const {
		return Vector3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	/**
	 * Returns the dot product (scalar product) of this vector with another one.
	 *
	 * @param v The vector to dot multiply with.
	 *
	 * @return The vectors' components multiplied pairwise and summed up.
	 */
	T dot(Vector3<T> v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	/**
	 * Returns the length of this vector as a floating point value.
	 *
	 * @return The square root of the vector's dot product with itself.
	 */
	float length() const {
		return std::sqrt(dot(*this));
	}

	/**
	 * Returns the length of this vector as a vector coordinate typed value.
	 *
	 * @return The square root of the vector's dot product with itself.
	 */
	T lengthTyped() const {
		return std::sqrt(dot(*this));
	}

	/**
	 * Returns a normalized float vector pointing in the same direction as this vector, forcing it
	 * to have a length of 1. If the vector is zero, the returned vector will be invalid.
	 *
	 * @return The vector divided by its length
	 */
	Vector3<float> norm() const {
		return Vector3<float>(*this) / length();
	}

	/**
	 * Returns a normalized coordinate typed vector pointing in the same direction as this vector,
	 * forcing it to have a length of 1. If the vector is zero, the returned vector will be invalid.
	 *
	 * @return The vector divided by its length
	 */
	Vector3<T> normTyped() const {
		return *this / lengthTyped();
	}

	/**
	 * Returns whether the vector is an invalid vector (any component is NaN).
	 *
	 * @return True if the vector is invalid, false otherwise.
	 */
	bool invalid() const {
		return x != x || y != y || z != z;
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
	 * The z component of the vector.
	 */
	T z;

	/**
	 * A zero vector (0, 0, 0).
	 */
	static const Vector3<T> Zero;

	/**
	 * An invalid vector (NaN, NaN, NaN).
	 */
	static const Vector3<T> Invalid;

	/**
	 * Applies a functor with a vector parameter to the vector and returns the result.
	 *
	 * @param v The vector to combine with this one
	 * @param func The function to apply to this and the passed vector
	 *
	 * @return The component-wise result of func(*this, v)
	 */
	template<typename F>
	Vector3<T> genericOperator(Vector3<T> v, F func) const {
		v.x = func(x, v.x);
		v.y = func(y, v.y);
		v.z = func(z, v.z);
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
	Vector3<T>& genericOperatorAssign(Vector3<T> v, F func) {
		x = func(x, v.x);
		y = func(y, v.y);
		z = func(z, v.z);
		return *this;
	}

	std::string toString() const {
		return QString("(%1|%2|%3)").arg(x).arg(y).arg(z).toStdString();
	}

#ifndef _MSC_VER
	operator Cartesian<T>() const {
		return Cartesian<T>(this->x, this->y, this->z);
	}
	operator Spherical<T>() const {
		return Spherical<T>(this->x, this->y, this->z);
	}
#endif
};

template<typename T>
const Vector3<T> Vector3<T>::Zero = Vector3<T>(0, 0, 0);
template<typename T>
const Vector3<T> Vector3<T>::Invalid = Vector3<T>(std::numeric_limits<T>::signaling_NaN(),
                                       std::numeric_limits<T>::signaling_NaN(), std::numeric_limits<T>::signaling_NaN());

/**
 * Negates the vector's components and returns the result.
 *
 * @param v The vector to negate
 *
 * @return The vector with its components negated
 */
template<typename T>
Vector3<T> operator-(Vector3<T> v) {
	return Vector3<T>(-v.x, -v.y, -v.z);
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
Vector3<T> operator+(Vector3<T> v, Vector3<T2> v2) {
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
Vector3<T>& operator+=(Vector3<T>& v, Vector3<T2> v2) {
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
Vector3<T> operator-(Vector3<T> v, Vector3<T2> v2) {
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
Vector3<T>& operator-=(Vector3<T>& v, Vector3<T2> v2) {
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
Vector3<T> operator*(Vector3<T> v, Vector3<T2> v2) {
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
Vector3<T>& operator*=(Vector3<T>& v, Vector3<T2> v2) {
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
Vector3<T> operator/(Vector3<T> v, Vector3<T2> v2) {
	if (v2.x == 0 || v2.y == 0 || v2.z == 0) {
		return Vector3<T>::Invalid;
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
Vector3<T>& operator/=(Vector3<T>& v, Vector3<T2> v2) {
	if (v2.x == 0 || v2.y == 0 || v2.z == 0) {
		return v = Vector3<T>::Invalid;
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
Vector3<T> operator*(Vector3<T> v, T2 s) {
	return Vector3<T>(v.x * s, v.y * s, v.z * s);
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
Vector3<T>& operator*=(Vector3<T>& v, T2 s) {
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
Vector3<T> operator/(Vector3<T> v, T2 s) {
	if (s == 0) {
		return Vector3<T>::Invalid;
	}
	return Vector3<T>(v.x / s, v.y / s, v.z / s);
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
Vector3<T>& operator/=(Vector3<T>& v, T2 s) {
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
Vector3<T> operator*(T2 s, Vector3<T> v) {
	return Vector3<T>(s * v.x, s * v.y, s * v.z);
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
Vector3<T> operator/(T2 s, Vector3<T> v) {
	if (v.x == 0 || v.y == 0 || v.z == 0) {
		return Vector3<T>::Invalid;
	}
	return Vector3<T>(s / v.x, s / v.y, s / v.z);
}

/**
 * Short-named type aliases for common vector component types.
 */
typedef Vector3<int> Vector3i;
typedef Vector3<unsigned int> Vector3u;
typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;

#ifndef _MSC_VER
template<typename T> class Spherical : public Vector3<T> {
public:
	using Vector3<T>::Vector3;
};
template<typename T> class Cartesian : public Vector3<T> {
public:
	using Vector3<T>::Vector3;
};
#else
template<typename T> using Spherical = Vector3<T>;
template<typename T> using Cartesian = Vector3<T>;
#endif

#endif
