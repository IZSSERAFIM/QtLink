#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QTimer>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
//#include "player.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int map[10][10];
    int map_prop[10][10];
    void newGame();
    void switchMode();
    int mode;
    bool playercanbeHelped;
    bool playerisinHelped;
    int propappearTime;
    //    int gameTime;
    bool pauseTime;
    void functionMenu();
    void createMap();
    static int mapBox;
    static int boxLength;
    void paintEvent(QPaintEvent *event) override;//绘图事件
    void keyPressEvent(QKeyEvent *event) override;//键盘事件
    //    void keyReleaseEvent(QKeyEvent *event) override;//键盘事件
    void movePlayer_1(QKeyEvent *event);//控制移动玩家一
    void movePlayer_2(QKeyEvent *event);//控制移动玩家二
    bool canHint();
    void eliminateHint();
    bool canLink(int px,int py,int cx,int cy);//两点可以被连接
    void drawLink(int px,int py,int cx,int cy);//画线
    bool isBlank_row(int px,int py,int cx,int cy);//同一行两点之间是否全空
    bool isBlank_line(int px,int py,int cx,int cy);//同一列两点之间是否全空
    bool canbyLine_adjacent(int px,int py,int cx,int cy);//相邻2个可以连接
    bool canbyLine(int px,int py,int cx,int cy);//两点可以直接一条线直接连接
    bool canbyOne(int px,int py,int cx,int cy);//两点通过一个拐点连接
    bool canbyTwo(int px,int py,int cx,int cy);//两点通过两个拐点连接
    void eliminate(int px,int py,int cx,int cy);//消除两个点
    void drawLine(int px,int py,int cx,int cy,bool type);//画(擦除)一条直线
    void drawLine_adjacent(int px,int py,int cx,int cy);//画相邻2点连线
    void drawbyOne(int px,int py,int cx,int cy);//画一个拐点的折线
    void drawbyTwo(int px,int py,int cx,int cy);//画两个拐点的折线
    void sleep(int time);
    void gameisOver();
    void endDialog();
    void plusTime();
    void shuffle();
    void openGame_test();//测试专用
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::MainWindow *ui;
    QPixmap icon[10][10];
    QTimer *timer;
    bool canProp;
    QList<int> keys;	//头文件中添加成员
    QTimer* keyRespondTimer;
    //    void slotTimeOut();

private slots:
    void pauseGame();
    void saveGame();
    void openGame();
    void quitGame();
    void timeEvent();
    void showScore();
    void dropProp();
    void startanewGame();

public slots:
    void hint();
    void launchHint();

};
#endif // MAINWINDOW_H
