
#ifndef KRONOS_UTILS_MISC_EXCEPTIONS_HPP
#define KRONOS_UTILS_MISC_EXCEPTIONS_HPP

#include <Utils/Misc/Macros.hpp>

struct KronosException : public std::exception {
	std::string reason;

	KronosException() { }
	KronosException(QString reason) : reason(reason.toStdString()) { }

	const char* what() const KRONOS_NOTHROW override {
		return reason.c_str();
	}
};

struct InvalidStateException : public KronosException {
	InvalidStateException(QString reason) : KronosException(reason) { }
};

#endif
