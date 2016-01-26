#ifndef KRONOS_TESTRUNNER_HPP
#define KRONOS_TESTRUNNER_HPP

#include <QObject>

class KronosTestRunner : public QObject {
	Q_OBJECT

public:
	KronosTestRunner(int argc, char** argv);
	~KronosTestRunner() { };

	int run();

private:
	int argc;
	char** argv;

private slots:
	void runTests();
};

#endif
