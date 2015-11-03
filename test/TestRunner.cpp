#include "TestRunner.hpp"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TextTestRunner.h>

#include <QCoreApplication>
#include <QTimer>

using namespace CppUnit;

KronosTestRunner::KronosTestRunner(int argc, char **argv) : argc(argc), argv(argv) { }

void KronosTestRunner::run() {
	// initialize and run a QApplication which is needed for some QT functionality
	QCoreApplication app(argc, argv);

	QTimer::singleShot(10, this, SLOT(runTests()));

	app.exec();
}

void KronosTestRunner::runTests() {
	TextTestRunner runner;
	TestFactoryRegistry &registry = TestFactoryRegistry::getRegistry();

	// run all tests if none specified on command line
	Test *testToRun = registry.makeTest();
	TestSuite *suite = dynamic_cast<TestSuite *>(testToRun);

	if (argc > 1) {
		testToRun = testToRun->findTest(argv[1]);
	}

	runner.addTest(testToRun);

	bool failed = runner.run();
	QCoreApplication::exit(failed);
}

int main(int argc, char *argv[]) {
	KronosTestRunner runner(argc, argv);
	runner.run();
}
