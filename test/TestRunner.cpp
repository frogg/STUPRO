#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TextTestRunner.h>

#include <iostream>

using namespace CppUnit;

int main(int argc, char *argv[])
{
	TextTestRunner runner;
	TestFactoryRegistry &registry = TestFactoryRegistry::getRegistry();

	// run all tests if none specified on command line
	Test *testToRun = registry.makeTest();
	TestSuite *suite = dynamic_cast<TestSuite *>(testToRun);

	if (argc > 1)
	{
		testToRun = testToRun->findTest(argv[1]);
	}

	runner.addTest(testToRun);

	bool failed = runner.run();
	return !failed;
}
