#include "TestRunner.hpp"

#include <gtest/gtest.h>
#include <thread>
#include <QCoreApplication>
#include <QTimer>
#include <Utils/Misc/KronosLogger.hpp>
#include <fstream>

KronosTestRunner::KronosTestRunner(int argc, char** argv) : argc(argc), argv(argv) { }

int KronosTestRunner::run() {
	// initialize and run a QApplication which is needed for some QT functionality
	QCoreApplication app(argc, argv);

	QTimer::singleShot(0, this, SLOT(runTests()));

	return app.exec();
}

void KronosTestRunner::runTests() {
	std::thread([this]() {
		testing::InitGoogleTest(&argc, argv);
		QCoreApplication::exit(RUN_ALL_TESTS());
	}).detach();
}

int main(int argc, char* argv[]) {
	// redirect the kronos logger output to a file
	std::ofstream out("kronos.log");
	setLoggerOutStream(out);

	// start the test runner
	KRONOS_LOG_INFO("Starting Test Runner");
	KronosTestRunner runner(argc, argv);
	int exitCode = runner.run();
	KRONOS_LOG_INFO("Test Runner finished");

	return exitCode;
}
