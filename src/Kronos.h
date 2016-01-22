#ifndef KRONOS_KRONOS_MAIN_H
#define KRONOS_KRONOS_MAIN_H

#include <QObject>

class Kronos : public QObject {
    Q_OBJECT;

public:
    Kronos(QObject* parent = 0);
    ~Kronos() {}

    void onStartup();
    void onShutdown();
};

#endif
