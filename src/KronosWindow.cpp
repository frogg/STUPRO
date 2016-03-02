#include <KronosWindow.hpp>
#include "ui_KronosWindow.h"

#include <pqHelpReaction.h>
#include <pqParaViewBehaviors.h>
#include <pqParaViewMenuBuilders.h>
#include <pqActiveObjects.h>
#include <pqObjectBuilder.h>
#include <pqApplicationCore.h>
#include <pqDeleteReaction.h>
#include <pqLoadDataReaction.h>

#include <qurl.h>
#include <qmainwindow.h>
#include <qnamespace.h>
#include <qstring.h>

/**
 * This class basically provides the default ParaView
 * components for initialization.
 */
class KronosWindow::pqInternals : public Ui::pqClientMainWindow {
};

KronosWindow::KronosWindow() {
	// Setup default GUI layout.
	this->Internals = new pqInternals();
	this->Internals->setupUi(this);

	// Set up the dock window corners to give the vertical docks more room.
	this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	this->Internals->animationViewDock->hide();
	this->Internals->statisticsDock->hide();
	this->Internals->comparativePanelDock->hide();
	this->tabifyDockWidget(this->Internals->animationViewDock,
	                       this->Internals->statisticsDock);

	// Enable help for from the object inspector.
	/*QObject::connect(this->Internals->proxyTabWidget,
		SIGNAL(helpRequested(QString)),
		this, SLOT(showHelpForProxy(const QString&)));*/

	// Populate application menus with actions.
	pqParaViewMenuBuilders::buildFileMenu(*this->Internals->menu_File);
	pqParaViewMenuBuilders::buildEditMenu(*this->Internals->menu_Edit);

	// Populate sources menu.
	pqParaViewMenuBuilders::buildSourcesMenu(*this->Internals->menuSources, this);

	// Populate filters menu.
	pqParaViewMenuBuilders::buildFiltersMenu(*this->Internals->menuFilters, this);

	// Populate Tools menu.
	pqParaViewMenuBuilders::buildToolsMenu(*this->Internals->menuTools);

	// setup the context menu for the pipeline browser.
	pqParaViewMenuBuilders::buildPipelineBrowserContextMenu(
	    *this->Internals->pipelineBrowser);

	pqParaViewMenuBuilders::buildToolbars(*this);

	// Setup the View menu. This must be setup after all toolbars and dockwidgets
	// have been created.
	pqParaViewMenuBuilders::buildViewMenu(*this->Internals->menu_View, *this);

	// Setup the menu to show macros.
	pqParaViewMenuBuilders::buildMacrosMenu(*this->Internals->menu_Macros);

	// Setup the help menu.
	pqParaViewMenuBuilders::buildHelpMenu(*this->Internals->menu_Help);

	// Final step, define application behaviors. Since we want all ParaView
	// behaviors, we use this convenience method.
	new pqParaViewBehaviors(this, this);

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
