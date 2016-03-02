#ifndef __KRONOSWINDOW_H
#define __KRONOSWINDOW_H

#include <qmainwindow.h>
#include <qobjectdefs.h>
#include <qstring.h>

#include <QDragEnterEvent>
#include <QDropEvent>

/**
 * This class is the main window used to start our
 * custom application.
 */
class KronosWindow : public QMainWindow {
	Q_OBJECT
	typedef QMainWindow Superclass;

public:
	KronosWindow();
	~KronosWindow();
	
protected:
	void dragEnterEvent(QDragEnterEvent *evt);
	void dropEvent(QDropEvent *evt);

protected slots:
	void showHelpForProxy(const QString& proxyname);

private:
	KronosWindow(const KronosWindow&); // Not implemented.
	void operator=(const KronosWindow&); // Not implemented.

	class pqInternals;
	pqInternals* Internals;
};

#endif
