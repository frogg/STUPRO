#include <KronosWindow.hpp>
#include "ui_KronosWindow.h"

#include "vtkPVConfig.h"
#include "pqActiveObjects.h"
#include "pqApplicationCore.h"
#include "pqDeleteReaction.h"
#include "pqLoadDataReaction.h"
#include "pqHelpReaction.h"
#include "pqOptions.h"
#include "pqParaViewBehaviors.h"
#include "pqParaViewMenuBuilders.h"
#include "pqSettings.h"
#include "vtkProcessModule.h"
#include "vtkPVGeneralSettings.h"
#include "vtkPVPlugin.h"

#ifndef BUILD_SHARED_LIBS
	#include "pvStaticPluginsInit.h"
#endif

#include <QDragEnterEvent>
#include <QDropEvent>
#include <qmimedata.h>
#include <qurl.h>
#include <qmainwindow.h>
#include <qnamespace.h>
#include <qstring.h>

#ifdef PARAVIEW_ENABLE_PYTHON
	#include "pqPythonDebugLeaksView.h"
	typedef pqPythonDebugLeaksView DebugLeaksViewType;
#else
	#include "vtkQtDebugLeaksView.h"
	typedef vtkQtDebugLeaksView DebugLeaksViewType;
#endif

/**
 * This class basically provides the default ParaView
 * components for initialization.
 */
class KronosWindow::pqInternals : public Ui::pqClientMainWindow { };

KronosWindow::KronosWindow() {
	if (getenv("PV_DEBUG_LEAKS_VIEW")) {
		vtkQtDebugLeaksView* leaksView = new DebugLeaksViewType(this);
		leaksView->setWindowFlags(Qt::Window);
		leaksView->show();
	}

	this->Internals = new pqInternals();
	this->Internals->setupUi(this);

	// Setup default GUI layout.
	this->setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);

	// Set up the dock window corners to give the vertical docks more room.
	this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	this->tabifyDockWidget(this->Internals->colorMapEditorDock, this->Internals->memoryInspectorDock);
	this->tabifyDockWidget(this->Internals->colorMapEditorDock, this->Internals->timeInspectorDock);
	this->tabifyDockWidget(this->Internals->colorMapEditorDock, this->Internals->comparativePanelDock);
	this->tabifyDockWidget(this->Internals->colorMapEditorDock, this->Internals->collaborationPanelDock);

	this->Internals->selectionDisplayDock->hide();
	this->Internals->animationViewDock->hide();
	this->Internals->statisticsDock->hide();
	this->Internals->comparativePanelDock->hide();
	this->Internals->collaborationPanelDock->hide();
	this->Internals->memoryInspectorDock->hide();
	this->Internals->multiBlockInspectorDock->hide();
	this->Internals->colorMapEditorDock->hide();
	this->Internals->timeInspectorDock->hide();

	this->tabifyDockWidget(this->Internals->animationViewDock, this->Internals->statisticsDock);

	// Set up the properties dock
	this->tabifyDockWidget(this->Internals->propertiesDock, this->Internals->viewPropertiesDock);
	this->tabifyDockWidget(this->Internals->propertiesDock, this->Internals->displayPropertiesDock);
	this->tabifyDockWidget(this->Internals->propertiesDock, this->Internals->informationDock);

	pqSettings *settings = pqApplicationCore::instance()->settings();
	int propertiesPanelMode = settings->value("GeneralSettings.PropertiesPanelMode", vtkPVGeneralSettings::ALL_IN_ONE).toInt();
	
	switch (propertiesPanelMode) {
	case vtkPVGeneralSettings::SEPARATE_DISPLAY_PROPERTIES:
		delete this->Internals->viewPropertiesPanel;
		delete this->Internals->viewPropertiesDock;
		this->Internals->viewPropertiesPanel = NULL;
		this->Internals->viewPropertiesDock = NULL;

		this->Internals->propertiesPanel->setPanelMode(
		pqPropertiesPanel::SOURCE_PROPERTIES|pqPropertiesPanel::VIEW_PROPERTIES);
		break;

	case vtkPVGeneralSettings::SEPARATE_VIEW_PROPERTIES:
		delete this->Internals->displayPropertiesPanel;
		delete this->Internals->displayPropertiesDock;
		this->Internals->displayPropertiesPanel = NULL;
		this->Internals->displayPropertiesDock = NULL;

		this->Internals->propertiesPanel->setPanelMode(pqPropertiesPanel::SOURCE_PROPERTIES|pqPropertiesPanel::DISPLAY_PROPERTIES);
		break;

	case vtkPVGeneralSettings::ALL_SEPARATE:
		this->Internals->propertiesPanel->setPanelMode(pqPropertiesPanel::SOURCE_PROPERTIES);
		break;

	case vtkPVGeneralSettings::ALL_IN_ONE:
	default:
		delete this->Internals->viewPropertiesPanel;
		delete this->Internals->viewPropertiesDock;
		this->Internals->viewPropertiesPanel = NULL;
		this->Internals->viewPropertiesDock = NULL;

		delete this->Internals->displayPropertiesPanel;
		delete this->Internals->displayPropertiesDock;
		this->Internals->displayPropertiesPanel = NULL;
		this->Internals->displayPropertiesDock = NULL;
		break;
	}

	this->Internals->propertiesDock->show();
	this->Internals->propertiesDock->raise();

	// Enable help from the properties panel.
	QObject::connect(this->Internals->propertiesPanel,
	SIGNAL(helpRequested(const QString&, const QString&)), this, SLOT(showHelpForProxy(const QString&, const QString&)));

	/// Hook delete to pqDeleteReaction.
	QAction* tempDeleteAction = new QAction(this);
	pqDeleteReaction* handler = new pqDeleteReaction(tempDeleteAction);
	handler->connect(this->Internals->propertiesPanel, SIGNAL(deleteRequested(pqPipelineSource*)), SLOT(deleteSource(pqPipelineSource*)));

	// Set up color editor
	/// Provide access to the color-editor panel for the application.
	pqApplicationCore::instance()->registerManager("COLOR_EDITOR_PANEL", this->Internals->colorMapEditorDock);

	// Populate application menus with actions.
	pqParaViewMenuBuilders::buildFileMenu(*this->Internals->menu_File);
	pqParaViewMenuBuilders::buildEditMenu(*this->Internals->menu_Edit);

	// Populate sources menu.
	pqParaViewMenuBuilders::buildSourcesMenu(*this->Internals->menuSources, this);

	// Populate filters menu.
	pqParaViewMenuBuilders::buildFiltersMenu(*this->Internals->menuFilters, this);

	// Populate Tools menu.
	pqParaViewMenuBuilders::buildToolsMenu(*this->Internals->menuTools);

	// Populate Catalyst menu.
	pqParaViewMenuBuilders::buildCatalystMenu(*this->Internals->menu_Catalyst);

	// Set up the context menu for the pipeline browser.
	pqParaViewMenuBuilders::buildPipelineBrowserContextMenu(*this->Internals->pipelineBrowser);

	pqParaViewMenuBuilders::buildToolbars(*this);

	// Set up the View menu. This must be setup after all toolbars and dockwidgets
	// have been created.
	pqParaViewMenuBuilders::buildViewMenu(*this->Internals->menu_View, *this);

	// Set up the menu to show macros.
	pqParaViewMenuBuilders::buildMacrosMenu(*this->Internals->menu_Macros);

	// Set up the help menu.
	pqParaViewMenuBuilders::buildHelpMenu(*this->Internals->menu_Help);

	// Final step, define application behaviors. Since we want all ParaView
	// behaviors, we use this convenience method.
	new pqParaViewBehaviors(this, this);
	
	// Enable help for from the object inspector.
	/*QObject::connect(this->Internals->proxyTabWidget,
		SIGNAL(helpRequested(QString)),
		this, SLOT(showHelpForProxy(const QString&)));*/

	// TODO: Try and set the default view to our view.
	// this->Internals->MultiViewWidget->closeTab(0);
	// pqApplicationCore::instance()->getObjectBuilder()->createView("StuproView",
	//         pqActiveObjects::instance().activeServer());
}

KronosWindow::~KronosWindow() {
	delete this->Internals;
}

void KronosWindow::showHelpForProxy(const QString& proxyname) {
	pqHelpReaction::showHelp(
	    QString("qthelp://paraview.org/paraview/%1.html").arg(proxyname));
}

void KronosWindow::dragEnterEvent(QDragEnterEvent *evt) {
	evt->acceptProposedAction();
}

void KronosWindow::dropEvent(QDropEvent *evt) {
	QList<QUrl> urls = evt->mimeData()->urls();
	if (urls.isEmpty()) {
		return;
	}

	QList<QString> files;

	foreach(QUrl url,urls) {
		if (!url.toLocalFile().isEmpty()) {
			files.append(url.toLocalFile());
		}
	}

	if (files.empty() || files.first().isEmpty()) {
		return;
	}
	
	pqLoadDataReaction::loadData(files);
}
