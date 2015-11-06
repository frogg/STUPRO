

#include "GlobeSwitcher.h"

#include <QApplication>
#include <QStyle>
#include <QMessageBox>

GlobeSwitcher::GlobeSwitcher(QObject* p)
  : QActionGroup(p)
{
  QIcon icon = qApp->style()->standardIcon(QStyle::SP_CustomBase);

  //defines the tooltip and the icon
  QAction* a = this->addAction(new QAction(icon, "Globeswitcher", this));
  QObject::connect(a, SIGNAL(triggered(bool)), this, SLOT(onAction()));
}

GlobeSwitcher::~GlobeSwitcher()
{
}

void GlobeSwitcher::onAction()
{
  QMessageBox::information(NULL, "Globeswitcher", "Globeswitcher needs to be implemented\n");
    
}

