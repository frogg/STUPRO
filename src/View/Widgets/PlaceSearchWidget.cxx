#include <View/Widgets/PlaceSearchWidget.h>

#include <Kronos.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QIcon>
#include <QSize>

#include <iostream>

PlaceSearchWidget::PlaceSearchWidget(QWidget * parent, Qt::WindowFlags flags)
        : QWidget(parent, flags) {
    // lay out the top level children vertically
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->setMargin(2);
    vLayout->setSpacing(2);

    // create a dummy widget that will contain the search box and search button
    QWidget* top = new QWidget(this);
    vLayout->addWidget(top);

    // lay out the widgets in the dummy widget horizontally
    QHBoxLayout *hLayout = new QHBoxLayout(top);
    hLayout->setMargin(0);
    hLayout->setSpacing(2);

    // initialize the search bar
    this->searchBar = new QLineEdit(top);
    // call the startSearch() method when the user presses the enter key
    this->connect(this->searchBar, SIGNAL(returnPressed()), SLOT(startSearch()));
    // add the search bar to the widget
    hLayout->addWidget(this->searchBar);

    // initialize the search button
    QPushButton* searchButton = new QPushButton(top);
    searchButton->setIcon(QIcon("./res/icons/search.png"));
    searchButton->setIconSize(QSize(16, 16));
    // call the startSearch() method when the button is clicked
    this->connect(searchButton, SIGNAL(clicked()), SLOT(startSearch()));
    // add the search button to the widget
    hLayout->addWidget(searchButton);

    // initialize the list that will later display the search results
    this->resultList = new QListView(this);
    this->resultList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // initialize the model that will later hold the search results
    this->resultListModel = new CityListModel(this);
    this->resultList->setModel(this->resultListModel);
    // add the result list to the widget
    vLayout->addWidget(this->resultList);

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
    this->resultListModel->clear();

    if (this->searchBar->text().length() == 0) {
        return;
    }

    std::vector<City> result;
    this->citiesDatabase->getCity(this->searchBar->text().toStdString(), &result);

    std::cout << "[startSearch] Place search for " << this->searchBar->text().toStdString() << " yielded ";
    if (result.size() > 0) {
        std::cout << result.size() << " results:" << std::endl;
        for (int i = 0; i < result.size(); i++) {
            std::cout << "[startSearch]\t" << result[i].name
                    << " [" << result[i].countryCode << "]"
                    << " " << result[i].latitude << "°N " << result[i].longitude << "°E" << std::endl;
            this->resultListModel->add(result[i]);
        }
    } else {
        std::cout << "no results" << std::endl;
    }

    // TODO: somehow make the camera focus on the looked up coordinates
    // TODO: if there was only one result. focus the camera on that location
    // TODO: if there are more than one result, wait until the user selects an item from the listview (double click or return)
    // TODO: decide what to do when the location couldn't be found
}
