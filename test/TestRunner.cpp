#include "TestRunner.hpp"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TextTestRunner.h>

#include <thread>
#include <QCoreApplication>
#include <QTimer>

using namespace CppUnit;

KronosTestRunner::KronosTestRunner(int argc, char **argv) : argc(argc), argv(argv) { }

int KronosTestRunner::run() {
	// initialize and run a QApplication which is needed for some QT functionality
	QCoreApplication app(argc, argv);

	QTimer::singleShot(0, this, SLOT(runTests()));

	return app.exec();
}

void KronosTestRunner::runTests() {
	std::thread([this](){
		TextTestRunner runner;
		TestFactoryRegistry &registry = TestFactoryRegistry::getRegistry();

		// run all tests if none specified on command line
		Test *testToRun = registry.makeTest();
		TestSuite *suite = dynamic_cast<TestSuite *>(testToRun);

		if (this->argc > 1) {
			testToRun = testToRun->findTest(this->argv[1]);
		}

		runner.addTest(testToRun);

		bool success = runner.run();
		QCoreApplication::exit(!success);
	}).detach();
}

int main(int argc, char *argv[]) {
	KronosTestRunner runner(argc, argv);
	return runner.run();
}
