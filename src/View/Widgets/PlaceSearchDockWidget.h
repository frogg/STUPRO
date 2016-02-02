#ifndef KRONOS_PLACE_SEARCH_DOCK_WIDGET_H
#define KRONOS_PLACE_SEARCH_DOCK_WIDGET_H

#include <View/Widgets/PlaceSearchWidget.h>

#include <QDockWidget>

class PlaceSearchDockWidget : public QDockWidget {
public:
    PlaceSearchDockWidget(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    ~PlaceSearchDockWidget() {}

private:
    PlaceSearchWidget* placeSearchWidget;
};

#endif
