#include <Utils/Misc/ServerUtils.hpp>

#include <pqApplicationCore.h>

bool ServerUtils::isServer() {
    return pqApplicationCore::instance() == nullptr;
}

bool ServerUtils::isClient() {
    return !ServerUtils::isServer();
}
