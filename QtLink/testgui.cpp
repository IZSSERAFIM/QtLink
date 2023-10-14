#include <QtTest/QtTest>
#include <QtGui>
#include <QLineEdit>
#include "testgui.h"

TestGui::TestGui(QObject *parent) : QObject(parent)
{

}
void TestGui::test_gui()
{
    QLineEdit lineEdit;
    QTest::keyClicks(&lineEdit, "hello world");
    QCOMPARE(lineEdit.text(), QString("hello world"));
}
void TestGui::test_gui_new_data()
{
    QTest::addColumn<QTestEventList>("events");
    QTest::addColumn<QString>("expected");
    QTestEventList list1;
    list1.addKeyClick('a');
    QTest::newRow("char")<<list1<<"a";
    QTestEventList list2;
    list2.addKeyClick('a');
    list2.addKeyClick(Qt::Key_Backspace);
    QTest::newRow("there and back again")<<list2<<"";
}
void TestGui::test_gui_new()
{
    QFETCH(QTestEventList,events);
    QFETCH(QString,expected);
    QLineEdit lineEdit;
    events.simulate(&lineEdit);
    QCOMPARE(lineEdit.text(),expected);
}
//QTEST_MAIN(TestGui)
