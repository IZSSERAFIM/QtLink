#include "simpletest.h"
#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include <QDir>
SimpleTest::SimpleTest(QObject *parent) : QObject(parent)
{

}

void SimpleTest::chooseBox_data()
{
    w.openGame_test();
    QTest::addColumn<int>("px");
    QTest::addColumn<int>("py");
    QTest::addColumn<int>("cx");
    QTest::addColumn<int>("cy");
    QTest::addColumn<bool>("result");
    //空和空
    QTest::newRow("NO.1")<<1<<1<<5<<1<<false;
    //空和块
    QTest::newRow("NO.2")<<1<<1<<3<<1<<false;
    //直线连接
    QTest::newRow("NO.3")<<2<<1<<2<<2<<false;
    QTest::newRow("NO.4")<<6<<7<<8<<7<<true;
    QTest::newRow("NO.5")<<3<<6<<3<<4<<false;
    QTest::newRow("NO.6")<<1<<3<<1<<6<<false;
    //一个拐点
    QTest::newRow("NO.7")<<5<<2<<8<<4<<false;
    QTest::newRow("NO.8")<<1<<7<<2<<8<<true;
    QTest::newRow("NO.9")<<3<<2<<4<<5<<false;
    //两个拐点
    QTest::newRow("NO.10")<<6<<5<<8<<7<<false;
    QTest::newRow("NO.11")<<6<<3<<8<<6<<false;
    QTest::newRow("NO.12")<<5<<2<<4<<5<<true;
    QTest::newRow("NO.13")<<8<<1<<8<<5<<false;
    QTest::newRow("NO.14")<<3<<1<<5<<4<<true;
    QTest::newRow("NO.15")<<3<<2<<4<<5<<false;



}
void SimpleTest::chooseBox()
{
    QFETCH(int,px);
    QFETCH(int,py);
    QFETCH(int,cx);
    QFETCH(int,cy);
    QFETCH(bool,result);
    QCOMPARE(w.canLink(px,py,cx,cy),result);
}
//QTEST_MAIN(SimpleTest)
