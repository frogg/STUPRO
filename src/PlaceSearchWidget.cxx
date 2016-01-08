#include <PlaceSearchWidget.h>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QIcon>
#include <QSize>

#include <iostream>

PlaceSearchWidget::PlaceSearchWidget(QWidget* parent)
        : QWidget(parent) {
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(2);

    this->searchBar = new QLineEdit(this);
    QObject::connect(this->searchBar, SIGNAL(returnPressed()), this, SLOT(startSearch()));
    layout->addWidget(this->searchBar);

    QPushButton* searchButton = new QPushButton(this);
    searchButton->setIcon(QIcon("./res/icons/search.png"));
    searchButton->setIconSize(QSize(16, 16));
    QObject::connect(searchButton, SIGNAL(clicked()), this, SLOT(startSearch()));
    layout->addWidget(searchButton);
}

void PlaceSearchWidget::startSearch() {
    // TODO: search for the entered location (this->searchBar->text())
    // TODO: somehow make the camera forcus on the looked up coordinates
    // TODO: decide what to do when the location couldn't be found

    std::cout << this->searchBar->text().toStdString() << std::endl;
}
