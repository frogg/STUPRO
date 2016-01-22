#ifndef KRONOS_KRONOS_MAIN_H
#define KRONOS_KRONOS_MAIN_H

#include <Utils/Database/CitiesDatabase.hpp>

#include <QObject>

class Kronos : public QObject {
    Q_OBJECT;

public:
    static Kronos* getInstance();

    Kronos(QObject* parent = 0);
    ~Kronos();

    /* listeners for paraview events */
    void onStartup();
    void onShutdown();

    /* getters for global objects */
    CitiesDatabase* getCitiesDatabase();
private:
    static Kronos* instance;

    bool initialized;
    CitiesDatabase* citiesDatabase;

    void ensureReady();
};

#endif
