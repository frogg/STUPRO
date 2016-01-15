#include <View/Widgets/PlaceSearchDockWidget.h>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QIcon>
#include <QSize>

PlaceSearchDockWidget::PlaceSearchDockWidget(QWidget * parent, Qt::WindowFlags flags)
        : QDockWidget("Find Places", parent, flags) {
    this->placeSearchWidget = new PlaceSearchWidget(this);
    this->setWidget(this->placeSearchWidget);
}
