#include <View/Widgets/PlaceSearchWidget.h>

#include <Kronos.h>
#include <Utils/Misc/KronosLogger.hpp>
#include <View/vtkSMStuproViewProxy.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QIcon>
#include <QSize>

#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkSMRenderViewProxy.h>

#include <pqActiveObjects.h>
#include <pqApplicationCore.h>
#include <pqRenderView.h>
#include <pqServerManagerModel.h>

#include <iostream>

PlaceSearchWidget::PlaceSearchWidget(QWidget * parent, Qt::WindowFlags flags)
        : QWidget(parent, flags)
{
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
    QObject::connect(
        this->resultList->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
        this, SLOT(listSelectionChanged(QItemSelection, QItemSelection))
    );
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
    this->resultList->selectionModel()->clearSelection();
    this->resultListModel->clear();

    if (this->searchBar->text().length() == 0) {
        return;
    }

    std::vector<City> result;
    this->citiesDatabase->getCity(this->searchBar->text().toStdString(), &result);

    if (result.size() > 0) {
        this->resultListModel->beginAdd(result.size());
        for (int i = 0; i < result.size(); i++) {
            this->resultListModel->add(result[i]);
        }
        this->resultListModel->endAdd();

        QModelIndex index = this->resultListModel->index(0);
        this->resultList->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
        this->resultList->setFocus();
    }
}

void PlaceSearchWidget::listSelectionChanged(const QItemSelection& selection, const QItemSelection& previous) {
    if (selection.indexes().size() == 1 && selection[0].height() == 1) {
        pqRenderView* rView = qobject_cast<pqRenderView*>(pqActiveObjects::instance().activeView());
        if (rView) {
            vtkSMRenderViewProxy* renderViewProxy = rView->getRenderViewProxy();
            vtkSMStuproViewProxy* stuproViewProxy = dynamic_cast<vtkSMStuproViewProxy*>(renderViewProxy);
            if (stuproViewProxy) {
                // get the selected city
                City city = this->resultListModel->getAll()[selection[0].top()];
                stuproViewProxy->moveCamera(city.latitude, city.longitude);
            }
        }
    }
}
