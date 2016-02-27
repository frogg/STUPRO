#include <Utils/Math/Functions.hpp>
#include <Utils/Misc/Macros.hpp>

double toRadians(double degrees) {
	return degrees * (double) (KRONOS_PI / 180.0);
}
