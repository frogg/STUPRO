#include <PlaceSearchWidget.h>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

#include <iostream>

PlaceSearchWidget::PlaceSearchWidget(QWidget* parent)
        : QWidget(parent) {
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);

    this->searchBar = new QLineEdit(this);
    QObject::connect(this->searchBar, SIGNAL(returnPressed()), this, SLOT(startSearch()));
    layout->addWidget(this->searchBar);

    QPushButton* searchButton = new QPushButton(this);
    QObject::connect(searchButton, SIGNAL(clicked()), this, SLOT(startSearch()));
    layout->addWidget(searchButton);
}

void PlaceSearchWidget::startSearch() {
    // TODO: search for the entered location (this->searchBar->text())
    // TODO: somehow make the camera forcus on the looked up coordinates
    // TODO: decide what to do when the location couldn't be found

    std::cout << this->searchBar->text().toStdString() << std::endl;
}
