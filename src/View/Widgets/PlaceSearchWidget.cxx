#include <View/Widgets/PlaceSearchWidget.h>

#include <Kronos.h>

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

    // open a database connection
    this->citiesDatabase = new CitiesDatabase("kronos","stuproUser","weloveparaview","127.0.0.1","5432");
    this->citiesDatabase->openDatabase();
}

PlaceSearchWidget::~PlaceSearchWidget() {
    this->citiesDatabase->closeDatabase();
    delete this->citiesDatabase;
    this->citiesDatabase = 0;
}

void PlaceSearchWidget::startSearch() {
    std::vector<City> result;
    this->citiesDatabase->getCity(this->searchBar->text().toStdString(), &result);

    std::cout << "[startSearch] Place search for " << this->searchBar->text().toStdString() << " yielded ";
    if (result.size() > 0) {
        std::cout << result.size() << " results:" << std::endl;
        for (int i = 0; i < result.size(); i++) {
            std::cout << "[startSearch]\t" << result[i].name << std::endl;
        }
    } else {
        std::cout << "no results" << std::endl;
    }

    // TODO: somehow make the camera focus on the looked up coordinates
    // TODO: decide what to do when the location couldn't be found
}
