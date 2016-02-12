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

/**
 * Returns the next (nearest upper) power of 2 of the specified unsigned integer.
 * 
 * Bit-level explanation: subtracts 1, copies highest set bit to all lower bits, then adds 1.
 * This causes the number to be rounded up to the next power of 2.
 * 
 * @param value The value to round up to the next power of 2.
 */
template <typename T>
T getNextPowerOf2(T value) {
	value--;
	for (unsigned int i = 1; i < sizeof(T) * 8; i <<= 1) {
		value |= value >> i;
	}
	return value + 1;
}

#endif
