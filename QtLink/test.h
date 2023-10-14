#ifndef TEST_H
#define TEST_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QtTest/QtTest>
#include <QtGui>
#include "player.h"
#include "mainwindow.h"

class test : public QObject
{
    Q_OBJECT
public:
    explicit test(QObject *parent = nullptr);

signals:

private slots:
    void toUpper_1();
    void toUpper_2();
    void toUpper_data();
    void toUpper();

};

#endif // TEST_H
