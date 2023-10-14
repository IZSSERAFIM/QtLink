#include "mainwindow.h"
#include "player.h"
#include <QApplication>
#include "test.h"

player player1,player2;


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    player1.w=&w;
    player2.w=&w;
    w.show();
    return a.exec();

}

