#ifndef KRONOS_PLACE_SEARCH_WIDGET_HPP
#define KRONOS_PLACE_SEARCH_WIDGET_HPP

#include <QLineEdit>
#include <QWidget>

class PlaceSearchWidget : public QWidget {
    Q_OBJECT

public:
    PlaceSearchWidget(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    ~PlaceSearchWidget() {}

private:
    QLineEdit* searchBar;

public slots:
    void startSearch();
};

#endif
