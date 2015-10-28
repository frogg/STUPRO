#include "testTestClass.hpp"

void TestTestClass::testMethod()
{
    TestClass test;
    QCOMPARE(test.testMethod(), 0);
}

QTEST_MAIN(TestTestClass)
