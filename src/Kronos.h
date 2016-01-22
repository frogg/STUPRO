#ifndef KRONOS_KRONOS_MAIN_H
#define KRONOS_KRONOS_MAIN_H

#include <Utils/Database/CitiesDatabase.hpp>
#include <View/vtkPVStuproView.h>

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

    /* getters/setters for class related properties */
    bool isInitialized();

    /* getters/setters for globally accessible objects */
    CitiesDatabase* getCitiesDatabase();

    void registerView(vtkPVStuproView* view);
    void unregisterView(vtkPVStuproView* view);
    std::vector<vtkPVStuproView*> getViews();

private:
    static Kronos* instance;

    bool initialized;
    CitiesDatabase* citiesDatabase;
    std::vector<vtkPVStuproView*> views;

    void ensureReady();
};

#endif
