#include "player.h"
#include <QGraphicsItem>

player::player(QObject *parent) : QObject(parent)
{
    score=0;//初始化分数为0
    playerhaveselectedOne=false;//玩家未选中方块
    oppo_canmove=true;//玩家对手可以移动
    oppo_isdizzy=false;//玩家对手未被眩晕
}
void player::dismiss_dizzy()
{
    oppo_isdizzy=0;
}
void player::dismiss_freeze()
{
    oppo_canmove=1;
}
void player::dismiss_hint()
{
    w->playercanbeHelped = 0;
    w->eliminateHint();
}
void player::initializePlayer(int number)
{
    playerNumber=number;//玩家数量设置为n(1/2)
    score=0;//初始化分数为0
    playerhaveselectedOne=false;//玩家未选中方块
    oppo_canmove=true;//玩家对手可以移动
    oppo_isdizzy=false;//玩家对手未被眩晕
    switch (number) {
    case 1:
        //初始化玩家一的出生位置
        x =200;
        y =250;
        break;
    case 2:
        //初始化玩家二的出生位置
        x =1750;
        y =250;
        break;
    }
}
void player::chooseBox()
{
    int i,j;
    if(x>=475&&y>=110){
        i= (x-475)/w->boxLength;//计算当前玩家所在方块的横坐标索引
        j= (y-110)/w->boxLength;//计算当前玩家所在方块的纵坐标索引
    }
    if(x<475&&y>=110){
        i= (x-w->boxLength-475)/w->boxLength;//计算当前玩家所在方块的横坐标索引
        j= (y-110)/w->boxLength;//计算当前玩家所在方块的纵坐标索引
    }
    if(x>=475&&y<110){
        i= (x-475)/w->boxLength;//计算当前玩家所在方块的横坐标索引
        j= (y-w->boxLength-110)/w->boxLength;//计算当前玩家所在方块的纵坐标索引
    }
    if(x<475&&y<110){
        i= (x-w->boxLength-475)/w->boxLength;//计算当前玩家所在方块的横坐标索引
        j= (y-w->boxLength-110)/w->boxLength;//计算当前玩家所在方块的纵坐标索引
    }
    qDebug()<<i<<","<<j<<endl;
    if(0<=i&&i<=9&&0<=j&&j<=9){
        if(w->map_prop[i][j]!=0){
            int help=w->map_prop[i][j];
            w->map_prop[i][j]=0;
            prop(help);
        }
    }
    int boxselected_x=-1,boxselected_y=-1;
    if(w->map[i][j]!=0){
        boxselected_x=i;
        boxselected_y=j;
    }
    if (boxselected_x >= 1 && boxselected_x <= w->mapBox-2 && boxselected_y >= 1 && boxselected_y <= w->mapBox-2)
        theboxSelected(boxselected_x,boxselected_y);
}
void player::theboxSelected(int x, int y)
{
    // 使用三目运算符判断是否为未选中或提示状态
    const bool isSelected = ((w->map[x][y] > 0 && w->map[x][y] < 10) || (w->map[x][y] > 20 && w->map[x][y] < 30));
    switch (isSelected) {
    case 1:
        // 根据当前玩家设置选中状态
        w->map[x][y] = (playerNumber == 1) ? 10 + (w->map[x][y] % 10) : 100 + (w->map[x][y] % 10);
        // 判断是否已经有选中的方块
        if (!playerhaveselectedOne) {
            // 之前未选中过
            previous_x = x;
            previous_y = y;
            playerhaveselectedOne = true;
        } else {
            // 调用 boxisLinked 进行链接操作
            boxisLinked(previous_x, previous_y, x, y);
            playerhaveselectedOne = false;
        }
        break;
    case 0:
        return;
    }
    w->gameisOver();
}

void player::prop(int helpType)
{
    switch(helpType){
    case 10001:
        w->plusTime();
        break;
    case 10000:
        w->shuffle();
        break;
    case 10002:
        w->playercanbeHelped = 1;
        QTimer::singleShot(10000,this,&player::dismiss_hint);
        break;
    case 10003:
        oppo_isdizzy = 1;
        QTimer::singleShot(10000,this,&player::dismiss_dizzy);
        break;
    case 10004:
        oppo_canmove = 0;
        QTimer::singleShot(3000,this,&player::dismiss_freeze);
        break;
    }
}
typedef void (player::*ActionFunction)(int, int, int, int);

void player::boxisLinked(int px, int py, int cx, int cy)
{
    ActionFunction action = nullptr;

    if (w->canLink(px, py, cx, cy)) {
        action = &player::doLink;
    } else {
        action = &player::doNotLink;
    }

    if (action != nullptr) {
        (this->*action)(px, py, cx, cy);
    }

    w->gameisOver();
}

void player::doLink(int px, int py, int cx, int cy)
{
    w->drawLink(px, py, cx, cy);
    score += w->map[px][py] % 10;
    w->eliminate(px, py, cx, cy);
}

void player::doNotLink(int px, int py, int cx, int cy)
{
    w->sleep(50);
    w->map[px][py] %= 10;
    w->map[cx][cy] %= 10;
}

