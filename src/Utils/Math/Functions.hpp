#ifndef KRONOS_MATH_FUNCTIONS_HPP
#define KRONOS_MATH_FUNCTIONS_HPP

template <typename T>
T absMod(T a, T b)
{
	return (a % b + b) % b;
}

#endif
