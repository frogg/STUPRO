#ifndef STUPRO_UTILS_HPP
#define STUPRO_UTILS_HPP

#include <string>

std::string readFile(std::string filename);

template <typename T>
T absMod(T a, T b)
{
	return (a % b + b) % b;
}

#endif
