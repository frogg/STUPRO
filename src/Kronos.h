#ifndef KRONOS_KRONOS_MAIN_H
#define KRONOS_KRONOS_MAIN_H

#include <View/KronosView.h>

#include <QObject>
#include <QList>

#include <functional>

class Kronos : public QObject {
	Q_OBJECT;

public:
	typedef std::function<void()> ShutdownHandler;

	static Kronos* getInstance();

	Kronos(QObject* parent = 0);
	~Kronos() {}


	/* listeners for paraview events */
	void onStartup();
	void onShutdown();

	/* getters/setters for class related properties */
	bool isInitialized();

	int registerShutdownHandler(ShutdownHandler handler);
	void unregisterShutdownHandler(int handlerId);

private:
	static Kronos* instance;

	bool initialized;

	struct ShutdownHandlerHandle {
		int id;
		ShutdownHandler handler;
		ShutdownHandlerHandle(ShutdownHandler handler) : handler(handler) {
			static int nextId = 0;
			this->id = nextId++;
		}
	};
	QList<ShutdownHandlerHandle> shutdownHandlers;
};

#endif
