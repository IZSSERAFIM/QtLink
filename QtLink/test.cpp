#include <QtTest/QtTest>
#include <QtGui>
#include <QLineEdit>
#include "test.h"
#include "player.h"
#include "mainwindow.h"

test::test(QObject *parent) : QObject(parent)
{

}

void test::toUpper_1()
{
    QString str = "Hello";
    QVERIFY(str.toUpper()=="HELLO");
}
void test::toUpper_2()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(),QString("HELLO"));
}
void test::toUpper_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");
    QTest::newRow("all lower")<<"hello"<<"HELLO";
    QTest::newRow("mixed")<<"Hello"<<"HELLO";
    QTest::newRow("all upper")<<"HELLO"<<"HELLO";
}
void test::toUpper()
{
    QFETCH(QString,string);
    QFETCH(QString,result);
    QCOMPARE(string.toUpper(),result);
}
//QTEST_MAIN(test)
