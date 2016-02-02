#ifndef KRONOS_KRONOS_MAIN_H
#define KRONOS_KRONOS_MAIN_H

#include <View/vtkPVStuproView.h>

#include <QObject>

class Kronos : public QObject {
    Q_OBJECT;

public:
    static Kronos* getInstance();

    Kronos(QObject* parent = 0);
    ~Kronos() {}


    /* listeners for paraview events */
    void onStartup();
    void onShutdown();

    /* getters/setters for class related properties */
    bool isInitialized();

private:
    static Kronos* instance;

    bool initialized;
};

#endif
