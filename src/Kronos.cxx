#include <Kronos.h>

#include <Utils/Misc/Exceptions.hpp>

#include <exception>
#include <iostream>
#include <QString>

Kronos* Kronos::instance = 0;

Kronos* Kronos::getInstance() {
    if (Kronos::instance == 0) {
        throw new InvalidStateException("No Kronos instance exists at the moment");
    }
    return Kronos::instance;
}



Kronos::Kronos(QObject* parent) : QObject(parent) {
    std::cout << "### KRONOS MAIN INIT ###" << std::endl;

    if (Kronos::instance != NULL) {
        throw new InvalidStateException("Only exactly one instance of Kronos may be initialized");
    }
    Kronos::instance = this;

    this->initialized = false;
}


void Kronos::onStartup() {
    std::cout << "### ON STARTUP ###" << std::endl;

    this->initialized = true;
}

void Kronos::onShutdown() {
    std::cout << "### ON SHUTDOWN ###" << std::endl;

    this->initialized = false;
}

bool Kronos::isInitialized() {
    return this->initialized;
}
