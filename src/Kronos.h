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

    /* getters/setters for globally accessible objects */
    void registerView(vtkPVStuproView* view);
    void unregisterView(vtkPVStuproView* view);
    std::vector<vtkPVStuproView*> getViews();

private:
    static Kronos* instance;

    bool initialized;
    std::vector<vtkPVStuproView*> views;

    void ensureReady();
};

#endif
