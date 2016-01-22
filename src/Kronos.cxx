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
}

bool Kronos::isInitialized() {
    return this->initialized;
}

void Kronos::registerView(vtkPVStuproView* view) {
    // check if the view is already registered
    bool alreadyRegistered = false;
    for (auto it = this->views.begin(); it < this->views.end(); ++it) {
        if (*it == view) {
            alreadyRegistered = true;
        }
    }
    if (alreadyRegistered) {
        std::cout << "VIEW ALREADY REGISTERED" << std::endl;
    } else {
        std::cout << "VIEW NOT YET REGISTERED" << std::endl;
        this->views.push_back(view);
    }
}

void Kronos::unregisterView(vtkPVStuproView* view) {
    for (auto it = this->views.begin(); it < this->views.end();) {
        if (*it == view) {
            std::cout << "VIEW UNREGISTERED" << std::endl;
            this->views.erase(it);
            break;
        } else {
            ++it;
        }
    }
}

std::vector<vtkPVStuproView*> Kronos::getViews() {
    return this->views;
}

void Kronos::ensureReady() {
    if (!this->isInitialized()) {
        throw new InvalidStateException("Kronos is not yet fully initialized");
    }
}
