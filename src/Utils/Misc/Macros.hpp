#ifndef KRONOS_MACROS_HPP
#define KRONOS_MACROS_HPP

/**
 * Operating system macros.
 *
 * Can be used to conditionally compile platform-specific code.
 */
#if defined(_WIN32)
	#define KRONOS_WINDOWS
#elif defined(__APPLE__)
	#define KRONOS_MACOS
#elif defined(__linux__)
	#define KRONOS_LINUX
#else
	#	error Unrecognized Operating System!
#endif

/**
 * Mathematical Pi constant.
 */
#define KRONOS_PI 3.141592653589793238462643383279502884L

/**
 * KRONOS_NOTHROW Macro.
 *
 * Usable at the end of a function declaration to guarantee that it will never throw exceptions.
 */
#if defined(KRONOS_WINDOWS)
	// Visual Studio 2013 doesn't support noexcept, so use a regular empty throw specifier instead.
	#define KRONOS_NOTHROW throw()
#else
	// G++ and Clang support noexcept, so use it instead of the deprecated throw() notation.
	#define KRONOS_NOTHROW noexcept
#endif

#define KRONOS_FRIEND_TEST(testCaseName,testName)\
	friend class testCaseName##_##testName##_Test

#endif
