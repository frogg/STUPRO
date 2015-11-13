#ifndef __StuproWindow_h 
#define __StuproWindow_h

#include <qmainwindow.h>
#include <qobjectdefs.h>
#include <qstring.h>

/// MainWindow for the default ParaView application.
class StuproWindow : public QMainWindow
{
  Q_OBJECT
  typedef QMainWindow Superclass;
public:
  StuproWindow();
  ~StuproWindow();

protected slots:
  void showHelpForProxy(const QString& proxyname);

private:
  StuproWindow(const StuproWindow&); // Not implemented.
  void operator=(const StuproWindow&); // Not implemented.

  class pqInternals;
  pqInternals* Internals;
};

#endif


