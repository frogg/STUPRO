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

#endif
