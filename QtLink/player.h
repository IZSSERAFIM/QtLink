#ifndef PLAYER_H
#define PLAYER_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QGraphicsItem>
#include "mainwindow.h"

class player : public QObject
{
    Q_OBJECT
public:
    explicit player(QObject *parent = nullptr);
    int x,y;
    int previous_x,previous_y;
    MainWindow *w;
    int score;
    void initializePlayer(int number);
    int playerNumber;
    bool oppo_canmove;
    bool oppo_isdizzy;
    //    bool haveselectedBox;
    void selectBox(int m);
    void theboxSelected(int x,int y);
    bool canbeHelped();
    void prop(int helpType);
    void chooseBox();
    bool playerhaveselectedOne;
    void boxisLinked(int px,int py,int cx,int cy);
    void doLink(int px,int py,int cx,int cy);
    void doNotLink(int px,int py,int cx,int cy);
public slots:
    void dismiss_freeze();
    void dismiss_dizzy();
    void dismiss_hint();

};

#endif // PLAYER_H
