#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TextTestRunner.h>

#include <iostream>
#include <thread>
#include <QApplication>

using namespace CppUnit;

int main(int argc, char *argv[]) {
	std::thread qtAppThread([ & ]() {
		// initialize and run a QApplication which is needed for some QT functionality
		QApplication app(argc, argv);
		app.exec();
	});

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

	qtAppThread.join(); // may never complete since the app.exec() call never returns
	return !failed;
}
