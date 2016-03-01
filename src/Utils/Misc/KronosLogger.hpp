#ifndef KRONOS_UTILS_KRONOS_LOGGER_HPP
#define KRONOS_UTILS_KRONOS_LOGGER_HPP

#include <iostream>
#include <QString>

// #define KRONOS_LOG_FILE_NAMES
#define KRONOS_ENABLE_LOGGING
#define KRONOS_LOGGING_ENABLE_COLORS

#define KRONOS_MAX_LOG_MSG_LENGTH 2048

#ifdef __GNUC__
	// only supported on GCC/G++ compilers
	#define FUNCTION_NAME __PRETTY_FUNCTION__
#elif _MSC_VER
	// only supported on MSVC compilers
	#define FUNCTION_NAME __FUNCSIG__
#else
	// always defined as of the C++11 standard
	#define FUNCTION_NAME __func__
#endif

#if defined(__linux__) && defined(KRONOS_LOGGING_ENABLE_COLORS)
	#define COLOR_DEFAULT "\033[0m"
	#define COLOR_GREY "\033[1;30m"
	#define COLOR_CYAN "\033[1;36m"
	#define COLOR_BLUE "\033[1;34m"
	#define COLOR_YELLOW "\033[1;33m"
	#define COLOR_RED "\033[1;31m"
#else
	#define COLOR_DEFAULT ""
	#define COLOR_GREY ""
	#define COLOR_CYAN ""
	#define COLOR_BLUE ""
	#define COLOR_YELLOW ""
	#define COLOR_RED ""
#endif

static std::ostream outStream(std::cout.rdbuf());

inline void setLoggerOutStream(std::ostream& stream) {
	outStream.rdbuf(stream.rdbuf());
}

template<typename... Args>
inline void kronos_log(const char* debugLevel, const char* function, const char* file, int line,
                       const char* message, Args... args) {
	char formatted[KRONOS_MAX_LOG_MSG_LENGTH];

#ifndef _MSC_VER
	snprintf(formatted, KRONOS_MAX_LOG_MSG_LENGTH, message, args...);
#else
	sprintf_s(formatted, KRONOS_MAX_LOG_MSG_LENGTH, message, args...);
#endif

	outStream << debugLevel << " "
	          << function
#ifdef KRONOS_LOG_FILE_NAMES
	          << " (" << file << ":" << line << ")"
#endif
	          << ": " << formatted
	          << COLOR_DEFAULT << std::endl;
};

template<typename... Args>
inline void kronos_log(const char* debugLevel, const char* function, const char* file, int line,
                       const QString message, Args... args) {
	kronos_log(debugLevel, function, file, line, message.toStdString().c_str(), args...);
};

#ifdef KRONOS_ENABLE_LOGGING
	#define KRONOS_LOG_VERBOSE(message, ...) kronos_log(COLOR_GREY   "[VERBOSE]" COLOR_DEFAULT, FUNCTION_NAME, __FILE__, __LINE__, message, ##__VA_ARGS__)
	#define KRONOS_LOG_DEBUG(message, ...)   kronos_log(COLOR_CYAN   "[ DEBUG ]" COLOR_DEFAULT, FUNCTION_NAME, __FILE__, __LINE__, message, ##__VA_ARGS__)
	#define KRONOS_LOG_INFO(message, ...)    kronos_log(COLOR_BLUE   "[ INFO  ]" COLOR_DEFAULT, FUNCTION_NAME, __FILE__, __LINE__, message, ##__VA_ARGS__)
	#define KRONOS_LOG_WARN(message, ...)    kronos_log(COLOR_YELLOW "[WARNING]" COLOR_DEFAULT, FUNCTION_NAME, __FILE__, __LINE__, message, ##__VA_ARGS__)
	#define KRONOS_LOG_ERROR(message, ...)   kronos_log(COLOR_RED    "[ ERROR ]" COLOR_DEFAULT, FUNCTION_NAME, __FILE__, __LINE__, message, ##__VA_ARGS__)
	#define KRONOS_LOG_FATAL(message, ...)   kronos_log(COLOR_RED    "[ FATAL ]"              , FUNCTION_NAME, __FILE__, __LINE__, message, ##__VA_ARGS__)
#else
	#define KRONOS_LOG_VERBOSE(message, ...) /* */
	#define KRONOS_LOG_DEBUG(message, ...) /* */
	#define KRONOS_LOG_INFO(message, ...) /* */
	#define KRONOS_LOG_WARN(message, ...) /* */
	#define KRONOS_LOG_ERROR(message, ...) /* */
	#define KRONOS_LOG_FATAL(message, ...) /* */
#endif

#ifdef __GNUC__
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

inline void handleSegfault(int sig) {
	int items = 10;
	void* array[items];
	size_t size;

	size = backtrace(array, items);

	fprintf(stderr, "Error: signal %d:\n", sig);

	backtrace_symbols_fd(array, size, STDERR_FILENO);
	exit(1);
}
#endif

inline void registerSegfaultHandler() {
#ifdef __GNUC__
	signal(SIGSEGV, handleSegfault);
	KRONOS_LOG_INFO("Registered segfault handler");
#endif
}

#endif
