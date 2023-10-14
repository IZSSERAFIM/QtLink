#ifndef TESTGUI_H
#define TESTGUI_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QtTest/QtTest>
#include <QtGui>
#include "player.h"
#include "mainwindow.h"


class TestGui : public QObject
{
    Q_OBJECT
public:
    explicit TestGui(QObject *parent = nullptr);

signals:

private slots:
    void test_gui();
    void test_gui_new_data();
    void test_gui_new();
};

#endif // TESTGUI_H
