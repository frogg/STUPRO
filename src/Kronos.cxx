#include <Kronos.h>

#include <Utils/Misc/Exceptions.hpp>
#include <Utils/Misc/KronosLogger.hpp>

#include <exception>
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

	registerSegfaultHandler();

	this->initialized = true;
}

void Kronos::onShutdown() {
	std::cout << "### ON SHUTDOWN ###" << std::endl;

	for (ShutdownHandlerHandle handle : this->shutdownHandlers) {
		try {
			handle.handler();
		} catch (...) { }
	}

	this->initialized = false;
}

bool Kronos::isInitialized() {
	return this->initialized;
}

int Kronos::registerShutdownHandler(ShutdownHandler handler) {
	ShutdownHandlerHandle handle(handler);
	this->shutdownHandlers.append(handle);
	return handle.id;
}

void Kronos::unregisterShutdownHandler(int handlerId) {
	QMutableListIterator<ShutdownHandlerHandle> iterator(this->shutdownHandlers);
	while (iterator.hasNext()) {
		if (iterator.next().id == handlerId) {
			iterator.remove();
			break;
		}
	}
}
