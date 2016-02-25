#ifndef KRONOS_UTILS_MATH_INTERPOLATOR_HPP
#define KRONOS_UTILS_MATH_INTERPOLATOR_HPP

#include <Utils/Math/Vector2.hpp>
#include <Utils/Misc/KronosLogger.hpp>

class Interpolator {
public:
	Interpolator() = delete;

	/*
	 * Linear interpolations
	 */

	template<typename T>
	static T linear(double t, T start, T end) {
		return start + t * (end - start);
	}

	/*
	 * Quadratic interpolations
	 */

	template<typename T>
	static T quadraticIn(double t, T start, T end) {
		return Interpolator::linear(t * t, start, end);
	}

	template<typename T>
	static T quadraticOut(double t, T start, T end) {
		return Interpolator::linear(2 * t - t * t, start, end);
	}

	template<typename T>
	static T quadraticInOut(double t, T start, T end) {
		T center = (start + end) / 2;
		t *= 2;

		if (t <= 1.0) {
			return Interpolator::quadraticIn(t, start, center);
		} else {
			t -= 1.0;
			return Interpolator::quadraticOut(t, center, end);
		}
	}

	/*
	 * Bezier interpolations
	 */

	template<typename T>
	static T quadraticBezier(double t, T start, T stop, T controlPoint) {
		T d1 = start + (t * (controlPoint - start));
		T d2 = controlPoint + (t * (stop - controlPoint));
		return d1 + (t * (d2 - d1));
	}

	template<typename T>
	static T qubicBezier(double t, T start, T stop, T controlPoint1, T controlPoint2) {
		// TODO: validate this calculation
		T d1 = start + (t * (controlPoint1 - start));
		T d2 = controlPoint1 + (t * (controlPoint2 - controlPoint1));
		T d3 = controlPoint2 + (t * (stop - controlPoint2));
		return Interpolator::quadraticBezier(t, d1, d3, d2);
	}
};

#endif // KRONOS_UTILS_MATH_INTERPOLATOR_HPP
