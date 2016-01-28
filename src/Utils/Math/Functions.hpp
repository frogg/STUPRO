#ifndef KRONOS_MATH_FUNCTIONS_HPP
#define KRONOS_MATH_FUNCTIONS_HPP

/**
 * Returns the positive modulo betwen the two specified numbers.
 *
 * @param a The number to divide
 * @param b The number to divide by
 *
 * @return The positive remainder of the division
 */
template <typename T>
T absoluteModulo(T a, T b) {
	return (a % b + b) % b;
}

/**
 * Linearly interpolates two values with an interpolation factor.
 *
 * @param a The number to mix for the interpolation value approaching 0.
 * @param b The number to mix for the interpolation value approaching 1.
 * @param interpolation The number to interpolate a and b with.
 */
template<typename T>
T interpolateLinear(T a, T b, T interpolation) {
	return interpolation * b + (1.f - interpolation) * a;
}

#endif
