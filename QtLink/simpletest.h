#ifndef SIMPLETEST_H
#define SIMPLETEST_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QtTest/QtTest>
#include <QtGui>
#include <QLineEdit>
#include "player.h"
#include "mainwindow.h"
class SimpleTest : public QObject
{
    Q_OBJECT
public:
    explicit SimpleTest(QObject *parent = nullptr);
    MainWindow w;
signals:

private slots:
    void chooseBox();
    void chooseBox_data();
};

#endif // SIMPLETEST_H
