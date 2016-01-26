#ifndef INCLUDEEIGEN_HPP
#define INCLUDEEIGEN_HPP

// disable deprecated warnings
#ifdef _MSC_VER
	#pragma warning(push, 0)
#elif defined __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <Eigen-v3.2.6/Dense>

// enable warnings again
#ifdef _MSC_VER
	#pragma warning(pop)
#elif defined __GNUC__
	#pragma GCC diagnostic pop
#endif

#endif // INCLUDEEIGEN_HPP

