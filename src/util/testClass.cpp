#include "testClass.hpp"

#include <iostream>
#include <QDebug>

int TestClass::testMethod()
{
	qDebug() << "test";
	return 0;
}
