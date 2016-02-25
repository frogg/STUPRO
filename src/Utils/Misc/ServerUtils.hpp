#ifndef KRONOS_UTILS_MISC_SERVER_UTILS_HPP
#define KRONOS_UTILS_MISC_SERVER_UTILS_HPP

class ServerUtils {
public:
    ServerUtils() = delete;

    static bool isClient();
    static bool isServer();
};

#endif // KRONOS_UTILS_MISC_SERVER_UTILS_HPP
