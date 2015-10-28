#ifndef KRONOS_TEST_TESTTESTCLASS_HPP
#define KRONOS_TEST_TESTTESTCLASS_HPP

#include <QtTest/QtTest>

#include <testClass.hpp>

class TestTestClass: public QObject
{
    Q_OBJECT
private slots:
    void testMethod();
};

#endif
