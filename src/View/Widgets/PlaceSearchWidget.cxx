#include <View/Widgets/PlaceSearchWidget.h>

#include <Utils/Config/Configuration.hpp>
#include <Utils/Misc/KronosLogger.hpp>
#include <View/vtkSMKronosViewProxy.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>


#include <pqActiveObjects.h>
#include <pqRenderView.h>

PlaceSearchWidget::PlaceSearchWidget(QWidget* parent, Qt::WindowFlags flags)
	: QWidget(parent, flags) {
	this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

	// lay out the top level children vertically
	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->setMargin(2);
	vLayout->setSpacing(2);

	// create a dummy widget that will contain the search box and search button
	QWidget* top = new QWidget(this);
	vLayout->addWidget(top);

	// lay out the widgets in the dummy widget horizontally
	QHBoxLayout* hLayout = new QHBoxLayout(top);
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
	this->resultList->setMinimumHeight(48);
	this->resultList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	this->resultList->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	// initialize the model that will later hold the search results
	this->resultListModel = new CityListModel(this);
	this->resultList->setModel(this->resultListModel);
	QObject::connect(
	    this->resultList->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
	    this, SLOT(listSelectionChanged(QItemSelection, QItemSelection))
	);
	// add the result list to the widget
	vLayout->addWidget(this->resultList);

	// try to open a database connection
	try {
		QString databaseName = Configuration::getInstance().getString("postgres.database");
		QString databaseUser = Configuration::getInstance().getString("postgres.user");
		QString databasePassword = Configuration::getInstance().getString("postgres.password");
		QString serverAddress = Configuration::getInstance().getString("postgres.serverAddress");
		int serverPort = Configuration::getInstance().getInteger("postgres.serverPort");
		this->citiesDatabase = new CitiesDatabase(databaseName, databaseUser, databasePassword,
		        serverAddress, serverPort);
		this->citiesDatabase->openDatabase();
	} catch (std::exception const& e) {
		KRONOS_LOG_WARN("Error opening a connection to the cities database: '%s'", e.what());
		this->connectionFailed();
	} catch (...) {
		KRONOS_LOG_WARN("Unknown error opening a connection to the cities database");
		this->connectionFailed();
	}
}

PlaceSearchWidget::~PlaceSearchWidget() {
	try {
		if (this->citiesDatabase) {
			this->citiesDatabase->closeDatabase();
		}
	} catch (...) { }

	delete this->citiesDatabase;
	this->citiesDatabase = 0;
}

void PlaceSearchWidget::connectionFailed() {
	KRONOS_LOG_WARN("PlaceSearchWidget will not be available");
	this->setEnabled(false);
	this->searchBar->setText("Failed to open a database connection");
}

void PlaceSearchWidget::startSearch() {
	this->resultList->selectionModel()->clearSelection();
	this->resultListModel->clear();

	if (this->searchBar->text().length() == 0) {
		return;
	}

	QList<City> cities = this->citiesDatabase->getCity(this->searchBar->text());

	if (cities.size() > 0) {
		this->resultListModel->beginAdd(cities.size());
		for (int i = 0; i < cities.size(); i++) {
			this->resultListModel->add(cities[i]);
		}
		this->resultListModel->endAdd();

		QModelIndex index = this->resultListModel->index(0);
		this->resultList->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
		this->resultList->setFocus();
	}
}

void PlaceSearchWidget::listSelectionChanged(const QItemSelection& selection,
        const QItemSelection& previous) {
	if (selection.indexes().size() == 1 && selection[0].height() == 1) {
		pqRenderView* rView = qobject_cast<pqRenderView*>(pqActiveObjects::instance().activeView());
		if (rView) {
			vtkSMRenderViewProxy* renderViewProxy = rView->getRenderViewProxy();
			vtkSMKronosViewProxy* kronosViewProxy = dynamic_cast<vtkSMKronosViewProxy*>(renderViewProxy);
			if (kronosViewProxy) {
				// get the selected city
				City city = this->resultListModel->getAll()[selection[0].top()];
				kronosViewProxy->moveCamera(city.latitude, city.longitude);
			}
		}
	}
}
