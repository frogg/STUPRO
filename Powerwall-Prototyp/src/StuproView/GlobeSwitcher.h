
#include <QActionGroup>

class GlobeSwitcher : public QActionGroup
{
  Q_OBJECT
public:
  GlobeSwitcher(QObject* p);
  ~GlobeSwitcher();

public slots:
  void onAction();

};

