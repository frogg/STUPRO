#ifndef STUPRO_ARITHMETICFUNCTORS_HPP
#define STUPRO_ARITHMETICFUNCTORS_HPP

/**
 * Implementation detail functors for Vector and Rect classes.
 */

/**
 * This functor adds two numbers of any type and returns the result.
 */
template <typename T, typename T2>
struct Add
{
	T operator()(T a, T2 b) const
	{
		return a + b;
	}
};

/**
 * This functor subtracts two numbers of any type and returns the result.
 */
template <typename T, typename T2>
struct Sub
{
	T operator()(T a, T2 b) const
	{
		return a - b;
	}
};

/**
 * This functor multiplies two numbers of any type and returns the result.
 */
template <typename T, typename T2>
struct Mul
{
	T operator()(T a, T2 b) const
	{
		return a * b;
	}
};

/**
 * This functor divides two numbers of any type and returns the result.
 */
template <typename T, typename T2>
struct Div
{
	T operator()(T a, T2 b) const
	{
		return a / b;
	}
};

#endif
