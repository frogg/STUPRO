#include <View/Widgets/PlaceSearchWidget.h>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QIcon>
#include <QSize>

#include <iostream>

PlaceSearchWidget::PlaceSearchWidget(QWidget * parent, Qt::WindowFlags flags)
        : QWidget(parent, flags) {
    // lay out the child widgets horizontally
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(2);

    // initialize the search bar itself
    this->searchBar = new QLineEdit(this);
    // call the startSearch() method when the user presses the enter key
    this->connect(this->searchBar, SIGNAL(returnPressed()), SLOT(startSearch()));
    // add the search bar to the widget
    layout->addWidget(this->searchBar);

    // initialize the search button
    QPushButton* searchButton = new QPushButton(this);
    searchButton->setIcon(QIcon("./res/icons/search.png"));
    searchButton->setIconSize(QSize(16, 16));
    // call the startSearch() method when the button is clicked
    this->connect(searchButton, SIGNAL(clicked()), SLOT(startSearch()));
    // add the search button to the widget
    layout->addWidget(searchButton);
}

void PlaceSearchWidget::startSearch() {
    // TODO: search for the entered location (this->searchBar->text())
    // TODO: somehow make the camera forcus on the looked up coordinates
    // TODO: decide what to do when the location couldn't be found

    std::cout << this->searchBar->text().toStdString() << std::endl;
}
