#include <Kronos.h>

#include <iostream>

Kronos::Kronos(QObject* parent) : QObject(parent) {
    std::cout << "### KRONOS MAIN INIT ###" << std::endl;
}

void Kronos::onStartup() {
    std::cout << "### ON STARTUP ###" << std::endl;
}

void Kronos::onShutdown() {
    std::cout << "### ON SHUTDOWN ###" << std::endl;
}
