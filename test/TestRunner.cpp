#include "TestRunner.hpp"

#include <gtest/gtest.h>
#include <thread>
#include <QCoreApplication>
#include <QTimer>

KronosTestRunner::KronosTestRunner(int argc, char **argv) : argc(argc), argv(argv) { }

int KronosTestRunner::run() {
	// initialize and run a QApplication which is needed for some QT functionality
	QCoreApplication app(argc, argv);

	QTimer::singleShot(0, this, SLOT(runTests()));

	return app.exec();
}

void KronosTestRunner::runTests() {
	std::thread([this](){
		testing::InitGoogleTest(&argc, argv);
		QCoreApplication::exit(RUN_ALL_TESTS());
	}).detach();
}

int main(int argc, char *argv[]) {
	KronosTestRunner runner(argc, argv);
	return runner.run();
}
