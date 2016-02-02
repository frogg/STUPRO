
#ifndef KRONOS_UTILS_MISC_EXCEPTIONS_HPP
#define KRONOS_UTILS_MISC_EXCEPTIONS_HPP

#include <Utils/Misc/Macros.hpp>

struct InvalidStateException : public std::exception {
	std::string reason;

	InvalidStateException(QString reason) : reason(reason.toStdString()) { }

	const char* what() const KRONOS_NOTHROW override {
		return reason.c_str();
	}
};

#endif
