#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include "player.h"
#include <QGraphicsItem>
#include <QFileDialog>
#include <random>
#include <algorithm>
#include <vector>
#include <ctime>

const int gameTime = 150000;//总时间150000ms
int MainWindow::mapBox=10;
int MainWindow::boxLength=100;
extern player player1,player2;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("QtLink");
    setWindowIcon(QIcon("://images/pl-main-logo.png"));
    this->setObjectName("mainWindow");
    this->setStyleSheet("#mainWindow{border-image:url(://images/new_background.jpg);}");
    newGame();
    functionMenu();
}
void MainWindow::newGame()
{
    //    // 如果timer已经被创建，则删除旧的timer对象
    //    if (timer) {
    //        delete timer;
    //    }
    switchMode();
    playercanbeHelped=0;
    propappearTime=0;
    //初始化进度条
    ui->timeBar->setMaximum(gameTime);
    ui->timeBar->setMinimum(0);
    ui->timeBar->setValue(gameTime);
    ui->timeBar->setStyleSheet("QProgressBar::chunk{background:green}");
    //初始化计分板
    ui->lcdNumber_1->setDigitCount(3);//设置数字显示的位数为3
    ui->lcdNumber_1->setMode(QLCDNumber::Dec);//数字显示模式设置为十进制
    ui->lcdNumber_1->setStyleSheet("background-color: #FABD1A");//设置玩家1记分牌背景颜色为橙色
    ui->lcdNumber_2->setDigitCount(3);
    ui->lcdNumber_2->setMode(QLCDNumber::Dec);
    ui->lcdNumber_2->setStyleSheet("background-color: #1A1BFA");//设置玩家2记分牌背景为深蓝色
    if (mode == 1)
        ui->lcdNumber_2->hide();//单人模式
    else
        ui->lcdNumber_2->show();//双人模式
    //创一个计时器
    timer = new QTimer(this);//创建一个QTimer对象timer用于触发定时任务
    connect(timer, &QTimer::timeout, this, [&](){
        update();
    });
    connect(timer,&QTimer::timeout,this,[&](){
        timeEvent();
    });
    connect(timer,&QTimer::timeout,this,[&](){
        showScore();
    });
    connect(timer,&QTimer::timeout,this,[&](){
        dropProp();
    });
    connect(timer, &QTimer::timeout, this,[&](){
        if(canHint())
            launchHint();
    });
    pauseTime=0;
    timer->start(10);//开始计时器并设定更新时间间隔为10ms
    createMap();
    //    if(playerhavebeenHelped){
    //        connect(timer, SIGNAL(timeout()), this, SLOT(hint()));//调用提示函数
    //    }
    //    gameisOver();
}
//选择模式
void MainWindow::switchMode()
{
    QDialog dialog(this);  // 创建对话框
    dialog.setWindowTitle("选择模式");
    dialog.setFixedSize(500,200);
    QPushButton *button1 = new QPushButton("单人模式", &dialog);  // 创建第一个按钮
    QPushButton *button2 = new QPushButton("双人模式", &dialog);  // 创建第二个按钮
    QVBoxLayout *layout = new QVBoxLayout(&dialog);  // 创建垂直布局
    layout->addWidget(button1);
    layout->addWidget(button2);
    // 连接按钮的点击事件
    connect(button1, &QPushButton::clicked, [&]() {
        mode = 1;
        dialog.accept();  // 接受对话框
    });
    connect(button2, &QPushButton::clicked, [&]() {
        mode = 2;
        dialog.accept();  // 接受对话框
    });
    // 显示对话框并等待用户选择
    if (dialog.exec() == QDialog::Rejected) {
        exit(0);  // 用户取消选择或关闭对话框时退出应用
    }
}
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    /*
        if (obj == ui->timeBar && event->type() == QEvent::Timer) {
            QProgressBar *timebar = qobject_cast<QProgressBar *>(obj);
            if (timebar->value() == 0) {
                timer->stop();
                QMessageBox::StandardButton result = QMessageBox::question(this, "游戏结束", "你输了!\n要再来一局吗？", QMessageBox::Yes | QMessageBox::No, QMessageBox::NoButton);
                if (result == QMessageBox::No)
                    QApplication::quit();
                newGame();
            } else {
                timebar->setValue(timebar->value() - 10);
                if (timebar->value() <= gameTime / 10)
                    timebar->setStyleSheet("QProgressBar::chunk{background:red}");
            }
        }
        */
    return QMainWindow::eventFilter(obj, event);
}
void MainWindow::timeEvent()
{
    if (ui->timeBar->value() == 0) // 时间耗尽
    {
        //        endDialog();
        switch (mode) {
        case 1:{
            timer->stop(); // 时间暂停
            QMessageBox::StandardButton result = QMessageBox::question(this, "游戏结束", QString::asprintf("你输了!\n你的得分是:%d\n要再来一局吗？",player1.score), QMessageBox::Yes | QMessageBox::No); // 弹出消息框
            if (result == QMessageBox::No)
                exit(0);
            else startanewGame();
            break;
        }
        case 2:{
            timer->stop();
            if(player1.score>player2.score){
                QMessageBox::StandardButton result =QMessageBox::question(this, "游戏结束",QString::asprintf("玩家1取胜!\n玩家1的得分是:%d\n玩家2的得分是:%d\n要再来一局吗？",player1.score,player2.score),QMessageBox::Yes | QMessageBox::No);
                if(result==QMessageBox::No)
                    exit(0);
                else    startanewGame();
            }
            if(player1.score<player2.score){
                QMessageBox::StandardButton result =QMessageBox::question(this, "游戏结束",QString::asprintf("玩家2取胜!\n玩家1的得分是:%d\n玩家2的得分是:%d\n要再来一局吗？",player1.score,player2.score),QMessageBox::Yes | QMessageBox::No);
                if(result==QMessageBox::No)
                    exit(0);
                else    startanewGame();
            }
            if(player1.score==player2.score){
                QMessageBox::StandardButton result =QMessageBox::question(this, "游戏结束",QString::asprintf("平局!\n玩家1的得分是:%d\n玩家2的得分是:%d\n要再来一局吗？",player1.score,player2.score),QMessageBox::Yes | QMessageBox::No);
                if(result==QMessageBox::No)
                    exit(0);
                else    startanewGame();
            }
            break;
        }
        }
    }
    else // 时间未耗尽
    {
        int currentValue = ui->timeBar->value();
        ui->timeBar->setValue(currentValue - 10);
        if (currentValue <= gameTime / 10)
            ui->timeBar->setStyleSheet("QProgressBar::chunk{background:red}");
    }
}
//暂停游戏
void MainWindow::pauseGame()
{
    switch (pauseTime) {
    case 0:
        timer->stop();
        pauseTime=true;
        ui->menu_pause->setTitle("继续");
        break;
    case 1:
        timer->start();
        pauseTime=false;
        ui->menu_pause->setTitle("暂停");
        break;
    }
}
//开始新一次游戏并释放上一次游戏的内存
void MainWindow::startanewGame()
{
    // 如果timer已经被创建，则删除旧的timer对象
    if (timer) {
        delete timer;
    }
    newGame();
}
//菜单按钮
void MainWindow::functionMenu()
{
    QAction *newAct = new QAction(tr("newGame"), this);
    ui->menu_newgame->addAction(newAct);
    connect(newAct, &QAction::triggered, this, &MainWindow::startanewGame);
    QAction *pauseAct = new QAction(tr("pauseGame"), this);
    ui->menu_pause->addAction(pauseAct);
    connect(pauseAct, &QAction::triggered, this, &MainWindow::pauseGame);
    QAction *openAct = new QAction(tr("openGame"), this);
    ui->menu_open->addAction(openAct);
    connect(openAct, &QAction::triggered, this, &MainWindow::openGame);
    QAction *saveasAct = new QAction(tr("saveGame"), this);
    ui->menu_save->addAction(saveasAct);
    connect(saveasAct, &QAction::triggered, this, &MainWindow::saveGame);
    QAction *quitAct = new QAction(tr("quitGame"), this);
    ui->menu_quit->addAction(quitAct);
    connect(quitAct, &QAction::triggered, this, &MainWindow::quitGame);
}
//退出游戏
void MainWindow::quitGame()
{
    timer->stop();
    QMessageBox::StandardButton result =QMessageBox::question(this, "游戏结束","再来一局?",QMessageBox::Yes | QMessageBox::No);
    if(result==QMessageBox::No){
        exit(0);
    }
    else{
        //        exit(0);
        //        QCoreApplication::quit();
        startanewGame();
    }
}
//创建地图元素数组
void MainWindow::createMap()
{
    srand(time(NULL));
    // 生成1到9的数字数组
    int nums[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    // 对数字数组进行洗牌
    std::random_shuffle(nums, nums + 9);
    int index = 0; // 数字数组索引
    /*
        // 初始化数字序列
        std::vector<int> numbers;
        // 添加1-9每个数字出现的偶数次
        for (int i = 1; i <= 9; ++i) {
             int count = 2 * (rand() % 5) + 2; // 生成一个偶数次，最大为10次
                for (int j = 0; j < count; ++j) {
                    numbers.push_back(i);
                }
        }
        // 打乱数字顺序
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(numbers.begin(), numbers.end(), g);
        //输出结果
        for (int i = 0; i < 64; ++i) {
             qDebug() << numbers[i] << " ";
        }
        */
    for (int i=0; i<mapBox; i++){
        for (int j=0; j<mapBox; j++){
            //地图边界全部设置为0，其余位置设置1~9
            if (i==0 || i==mapBox-1 || j==0 || j==mapBox-1 ){
                map[i][j]=0;
            }
            else{
                map[i][j] = rand()%9+1;
                //                map[i][j] = nums[index % 9]; // 使用洗牌后的数字数组来填充地图元素数组
                //                index++;
                //                map[i][j]=nums[rand()%9];
                //                map[i][j]=numbers[i*j-1];
            }
        }
    }
    //设置道具地图数组
    for (int i=0; i<mapBox; i++){
        for (int j=0; j<mapBox; j++){
            map_prop[i][j] = 0;
        }
    }
    /*for(int i=0;i<mapBox;i++)
            for(int j=0;j<mapBox;j++)
                qDebug()<<map[i][j];*/
    //初始化玩家位置
    switch (mode) {
    case 1:
        player1.initializePlayer(1);
        break;
    case 2:
        player1.initializePlayer(1);
        player2.initializePlayer(2);
        break;
    }
}
//进行绘图操作
void MainWindow::paintEvent(QPaintEvent *)
{
    //创建一个名为painter的QPainter的对象并与当前窗口关联
    QPainter painter(this);
    for (int i=0; i<mapBox; i++)
        for (int j=0; j<mapBox; j++)
        {
            //显示地图，从整个窗口的（475，110）开始画方块
            int x = boxLength*i+475;
            int y = boxLength*j+110;
            if (map_prop[i][j] != 0)
                //如果道具地图元素的值不为0，就使用map_prop[i][j]的值作为文件名的一部分，通过QString::asprintf()函数生成图标文件的路径，并将该路径传递给load()函数，加载图标文件到icons[i][j]中
                icon[i][j].load(QString("://images/%1.png").arg(map_prop[i][j]));
            else
                //如果道具地图元素的值为0，表示该位置没有道具，需要根据map[i][j]的值来确定图标文件的路径。同样使用QString::asprintf()函数生成图标文件的路径，并将路径传递给load()函数，加载图标文件到icons[i][j]中
                icon[i][j].load(QString("://images/%1.png").arg(map[i][j]));
            //在地图上指定位置绘制方块/道具
            painter.drawPixmap(x,y, boxLength, boxLength, icon[i][j]);
        }
    switch (mode) {
    case 1:
        //绘制玩家1
        painter.drawImage(QRect(player1.x,player1.y,40,40),QImage("://images/player1-removebg-preview.png"));
        break;
    case 2:
        painter.drawImage(QRect(player1.x,player1.y,40,40),QImage("://images/player1-removebg-preview.png"));
        painter.drawImage(QRect(player2.x,player2.y,40,40),QImage("://images/player2-removebg-preview.png"));
        break;
    }
    painter.end();
}
//通过调用movePlayer_1/2函数实现玩家移动
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //    if(!event->isAutoRepeat())  //判断如果不是长按时自动触发的按下,就将key值加入容器
    //           keys.append(event->key());
    //       if(!keyRespondTimer->isActive()) //如果定时器不在运行，就启动一下
    //           keyRespondTimer->start(4);
    switch (pauseTime) {
    case 1:
        return;
    case 0:
        if (player2.oppo_canmove){
            //控制移动player1
            movePlayer_1(event);
        }
        if (player1.oppo_canmove){
            //player2
            movePlayer_2(event);
        }
    }
}
//void MainWindow::keyReleaseEvent(QKeyEvent *event){
//    if(!event->isAutoRepeat())  //判断如果不是长按时自动触发的释放,就将key值从容器中删除
//        keys.removeAll(event->key());
//    if(keys.isEmpty()) //容器空了，关闭定时器
//        keyRespondTimer->stop();
//}

//实现玩家移动的函数
void MainWindow::movePlayer_1(QKeyEvent *event)
{
    int step = 10; // 调整每次移动的步长
    if (!player2.oppo_isdizzy){
        //玩家1未被眩晕
        switch (event->key()) {
        case Qt::Key_A:
            player1.x-=step;
            player1.chooseBox();
            break;
        case Qt::Key_D:
            player1.x+=step;
            player1.chooseBox();
            break;
        case Qt::Key_W:
            player1.y-=step;
            player1.chooseBox();
            break;
        case Qt::Key_S:
            player1.y+=step;
            player1.chooseBox();
            break;
        }
    }
    else{
        //玩家1被眩晕，上下左右移动逻辑颠倒
        switch (event->key()) {
        case Qt::Key_D:
            player1.x-=step;
            player1.chooseBox();
            break;
        case Qt::Key_A:
            player1.x+=step;
            player1.chooseBox();
            break;
        case Qt::Key_S:
            player1.y-=step;
            player1.chooseBox();
            break;
        case Qt::Key_W:
            player1.y+=step;
            player1.chooseBox();
            break;
        }
    }
}
void MainWindow::movePlayer_2(QKeyEvent *event){
    int step=10;
    if (!player1.oppo_isdizzy){
        switch (event->key()) {
        case Qt::Key_Left:
            player2.chooseBox();
            player2.x -=step;
            break;
        case Qt::Key_Right:
            player2.chooseBox();
            player2.x +=step;
            break;
        case Qt::Key_Up:
            player2.chooseBox();
            player2.y -=step;
            break;
        case Qt::Key_Down:
            player2.chooseBox();
            player2.y +=step;
            break;
        }
    }
    else{
        switch (event->key()) {
        case Qt::Key_Right:
            player2.chooseBox();
            player2.x -=step;
            break;
        case Qt::Key_Left:
            player2.chooseBox();
            player2.x +=step;
            break;
        case Qt::Key_Down:
            player2.chooseBox();
            player2.y -=step;
            break;
        case Qt::Key_Up:
            player2.chooseBox();
            player2.y +=step;
            break;
        }
    }
}
void MainWindow::saveGame()
{
    pauseGame();
    //    QString fileName = QFileDialog::getSaveFileName(this, tr("保存游戏"));
    QFileDialog dialog(this, tr("保存游戏"));
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec()) {
        QString fileName = dialog.selectedFiles().first();
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            // 写入文件的逻辑
            QDataStream out(&file);
            //通过双重循环遍历map和map_prop数组，使用out对象将数组中的数据写入文件
            for (int i=0; i<mapBox; i++){
                for (int j=0; j<mapBox; j++){
                    out << map[i][j]<< map_prop[i][j];
                }
            }
            //将界面上的时间进度条的值保存到value变量中，以及模式mode，并将其写入文件
            int timeValue;
            timeValue = ui->timeBar->value();
            out << timeValue << mode ;
            switch (mode) {
            case 1:
                //写入玩家的数据
                out << player1.x << player1.y << player1.previous_x << player1.previous_y << player1.playerhaveselectedOne << player1.score;
                break;
            case 2:
                out << player1.x << player1.y << player1.previous_x << player1.previous_y << player1.playerhaveselectedOne << player1.score;
                out << player2.x << player2.y << player2.previous_x << player2.previous_y << player2.playerhaveselectedOne << player2.score;
                break;
            }
            file.close();
        }
        else {
            QMessageBox::information(this, tr("创建失败"), file.errorString());
        }
    }
}
void MainWindow::openGame()
{
    if(!pauseTime){
        pauseGame();
    }
    QFileDialog dialog(this, tr("打开游戏"));
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    if (dialog.exec()) {
        QString fileName = dialog.selectedFiles().first();
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            // 写入文件的逻辑
            QDataStream in(&file);
            //读取map和map_prop数组
            for (int i=0; i<mapBox; i++)
                for (int j=0; j<mapBox; j++){
                    in >> map[i][j] >> map_prop[i][j];
                }
            //读取时间进度、模式及玩家数据
            int timeValue;
            in >>timeValue>>mode;
            switch (mode) {
            case 1:
                //写入玩家的数据
                in >> player1.x >> player1.y >> player1.previous_x >> player1.previous_y >> player1.playerhaveselectedOne >> player1.score;
                ui->lcdNumber_2->hide();
                break;
            case 2:
                //写入玩家的数据
                in >> player1.x >> player1.y >> player1.previous_x >> player1.previous_y >> player1.playerhaveselectedOne >> player1.score;
                in >> player2.x >> player2.y >> player2.previous_x >> player2.previous_y >> player2.playerhaveselectedOne >> player2.score;
                ui->lcdNumber_2->show();
                break;
            }
            file.close();
            ui->timeBar->setValue(timeValue);
            ui->lcdNumber_1->display(player1.score);
            ui->lcdNumber_1->display(player2.score);
        }
        else {
            QMessageBox::information(this, tr("打开失败"), file.errorString());
        }
    }
}
void MainWindow::openGame_test()
{
    pauseGame();
    QFileDialog dialog(this, tr("打开游戏"));
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    if (dialog.exec()) {
        QString fileName = dialog.selectedFiles().first();
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            // 写入文件的逻辑
            QDataStream in(&file);
            //读取map和map_prop数组
            for (int i=0; i<mapBox; i++)
                for (int j=0; j<mapBox; j++){
                    in >> map[i][j] >> map_prop[i][j];
                }
            //读取时间进度、模式及玩家数据
            int timeValue;
            in >>timeValue>>mode;
            switch (mode) {
            case 1:
                //写入玩家的数据
                in >> player1.x >> player1.y >> player1.previous_x >> player1.previous_y >> player1.playerhaveselectedOne >> player1.score;
                ui->lcdNumber_2->hide();
                break;
            case 2:
                //写入玩家的数据
                in >> player1.x >> player1.y >> player1.previous_x >> player1.previous_y >> player1.playerhaveselectedOne >> player1.score;
                in >> player2.x >> player2.y >> player2.previous_x >> player2.previous_y >> player2.playerhaveselectedOne >> player2.score;
                ui->lcdNumber_2->show();
                break;
            }
            file.close();
            ui->timeBar->setValue(timeValue);
            ui->lcdNumber_1->display(player1.score);
            ui->lcdNumber_1->display(player2.score);
        }
        else {
            QMessageBox::information(this, tr("打开失败"), file.errorString());
        }
    }

}
//实现显示分数
void MainWindow::showScore()
{
    //显示分数
    ui->lcdNumber_1->display(player1.score);
    ui->lcdNumber_2->display(player2.score);

}
void MainWindow::eliminate(int px, int py, int cx, int cy)
{
    map[px][py]=0;
    map[cx][cy]=0;
}
bool MainWindow::isBlank_row(int px, int py, int cx, int cy)
{
    if(py!=cy)
        return false;
    else {
        if(px>cx){
            std::swap(px,cx);//确保前一点在当前点左侧
        }
        for(int i=px+1;i<cx;i++){
            if(map[i][py]!=0||map_prop[i][py]!=0)
                return false;
        }
        return true;
    }
    return false;
}
bool MainWindow::isBlank_line(int px, int py, int cx, int cy)
{
    if(px!=cx)
        return false;
    else {
        if(py>cy){
            std::swap(py,cy);//确保前一点在当前点上方
        }
        for(int i=py+1;i<cy;i++){
            if(map[px][i]!=0||map_prop[px][i]!=0)
                return false;
        }
        return true;
    }
    return false;
}
bool MainWindow::canLink(int px, int py, int cx, int cy)
{
    if((map[px][py]%10)!=(map[cx][cy]%10)||(px==cx&&py==cy)){
        return false;//不同图案或者自己不能连接
    }
    else{
        if(((map[px-1][py]!=0||map_prop[px-1][py]!=0)&&(map[px+1][py]!=0||map_prop[px+1][py]!=0)&&(map[px][py-1]!=0||map_prop[px][py-1]!=0)&&(map[px][py+1]!=0||map_prop[px][py+1]))||((map[cx-1][cy]!=0||map_prop[cx-1][cy]!=0)&&(map[cx+1][cy]!=0||map_prop[cx+1][cy]!=0)&&(map[cx][cy-1]!=0||map_prop[cx][cy-1])&&(map[cx][cy+1]!=0||map_prop[cx][cy+1]!=0)))
            return false;//如果两个点中有一个的周围都被其他图案占据，则直接返回 false
        //两个点的周围有空位则通过canbyLine、canbyOne和canbyTwo函数依次判断两个点之间是否存在水平线、垂直线、一个拐点和两个拐点的路径，以确定是否能够进行连接。如果没有符合条件的路径，则返回false
        if (!canbyLine_adjacent(px,py,cx,cy) && !canbyLine(px,py,cx,cy) && !canbyOne(px,py,cx,cy) && !canbyTwo(px,py,cx,cy)){
            return false;
        }
        else{
            //两点之间可以连接
            return true;
        }
    }
}
bool MainWindow::canbyLine_adjacent(int px, int py, int cx, int cy)
{
    if((px==cx&&abs(py-cy)==1)||(py==cy&&abs(px-cx)==1)){
        if(px==cx&&abs(py-cy)==1){//一上一下
            if(map[px+1][py]==0&&map_prop[px+1][py]==0&&map[cx+1][cy]==0&&map_prop[cx+1][cy]==0)//右侧可连线
                return true;
            if(map[px-1][py]==0&&map_prop[px-1][py]==0&&map[cx-1][cy]==0&&map_prop[cx-1][cy]==0)//左侧可连线
                return true;
        }
        if(py==cy&&abs(px-cx)==1){//一左一右
            if(map[px][py+1]==0&&map_prop[px][py+1]==0&&map[cx][cy+1]==0&&map_prop[cx][cy+1]==0)//下方可连线
                return true;
            if(map[px][py-1]==0&&map_prop[px][py-1]==0&&map[cx][cy-1]==0&&map_prop[cx][cy-1]==0)//上方可连线
                return true;
        }
    }
    return false;
}
bool MainWindow::canbyLine(int px, int py, int cx, int cy)
{
    if(px==cx){//同一竖线
        if(abs(py-cy)==1) return false;
        if(py>cy){
            std::swap(py,cy);//确保前一点在当前点上方
        }
        return isBlank_line(px,py,cx,cy);
    }
    if (py==cy){//同一水平线
        if(abs(px-cx)==1) return false;
        if (px>cx){
            std::swap(px,cx);//确保前一点在当前点左侧
        }
        return isBlank_row(px,py,cx,cy);
    }
    return false;
}
bool MainWindow::canbyOne(int px, int py, int cx, int cy)
{
    if(px>cx){
        std::swap(px,cx);
        std::swap(py,cy);
    }//使前一点位于当前点左侧
    if(py<cy){//前一点在当前点左上
        if(cx-px==1&&cy-py==1){
            if((map[px][cy]==0&&map_prop[px][cy]==0)||(map[cx][py]==0&&map_prop[cx][py]==0))
                return true;
        }
        if(cx-px==1&&cy-py!=1){
            if((map[px][cy]==0&&map_prop[px][cy]==0&&isBlank_line(px,py,px,cy))||(map[cx][py]==0&&map_prop[cx][py]==0&&isBlank_line(cx,py,cx,cy)))
                return true;
        }
        if(cy-py==1&&cx-px!=1){
            if((map[px][cy]==0&&map_prop[px][cy]==0&&isBlank_row(px,cy,cx,cy))||(map[cx][py]==0&&map_prop[cx][py]==0&&isBlank_row(px,py,cx,py)))
                return true;
        }
        else {
            if(canbyLine(px,py,cx,py)&&canbyLine(cx,py,cx,cy)&&map[cx][py]==0&&map_prop[cx][py]==0)//→↓
                return true;
            if(canbyLine(px,py,px,cy)&&canbyLine(px,cy,cx,cy)&&map[px][cy]==0&&map_prop[px][cy]==0)//↓→
                return true;
        }
    }
    if(py>cy){//前一点在当前点左下
        if(cx-px==1&&py-cy==1){
            if((map[px][cy]==0&&map_prop[px][cy]==0)||(map[cx][py]==0&&map_prop[cx][py]==0))
                return true;
        }
        if(cx-px==1&&py-cy!=0){
            if((map[px][cy]==0&&map_prop[px][cy]==0&&isBlank_line(px,py,px,cy))||(map[cx][py]==0&&map_prop[cx][py]==0&&isBlank_line(cx,py,cx,cy)))
                return true;
        }
        if(py-cy==1&&cx-px!=0){
            if((map[px][cy]==0&&map_prop[px][cy]==0&&isBlank_row(px,cy,cx,cy))||(map[cx][py]==0&&map_prop[cx][py]==0&&isBlank_row(px,py,cx,py)))
                return true;
        }
        else {
            if(canbyLine(px,py,px,cy)&&canbyLine(px,cy,cx,cy)&&map[px][cy]==0&&map_prop[px][cy]==0)//↑→
                return true;
            if(canbyLine(px,py,cx,py)&&canbyLine(cx,py,cx,cy)&&map[cx][py]==0&&map_prop[cx][py]==0)//→↑
                return true;
        }
    }
    return false;
}
bool MainWindow::canbyTwo(int px, int py, int cx, int cy)
{
    if(px>cx){
        std::swap(px,cx);
        std::swap(py,cy);
    }//使前一点位于当前点左侧
    for (int i=0; i<mapBox; i++){
        if (i==py || i==cy)
            continue;
        bool b = true;//找到一条不同于fy与cy的水平线
        for (int j=px; j<=cx; j++){
            if (map[j][i]!=0||map_prop[j][i]!=0){
                b = false;//水平线上有方块元素值不为0
            }
        }
        if (b)//水平线上方块元素值全为0（全空）
            if (isBlank_line(px,py,px,i) && isBlank_line(cx,cy,cx,i))//两条竖直线
                return true;
    }
    if(py>cy){
        std::swap(px,cx);
        std::swap(py,cy);
    }//使前一点位于当前点上方
    for (int i=0; i<mapBox; i++){
        if (i==px || i==cx)
            continue;
        bool b = true;//找到一条不同于px与cx的垂直线
        for (int j=py; j<=cy; j++){
            if (map[i][j]!=0||map_prop[i][j]!=0){
                b = false;//垂直线上有方块元素值不为0
            }
        }
        if (b)//垂直线上方块元素值全为0（全空）
            if (isBlank_row(px,py,i,py) && isBlank_row(cx,cy,i,cy))//两条水平线
                return true;
    }
    return false;
}
void MainWindow::drawLink(int px, int py, int cx, int cy)
{
    if (canbyLine(px,py,cx,cy)){
        drawLine(px,py,cx,cy,1);//绘制直线
        sleep(80);//延迟80ms
        drawLine(px,py,cx,cy,0);//擦除直线
        return;
    }
    if (canbyLine_adjacent(px,py,cx,cy)){
        drawLine_adjacent(px,py,cx,cy);
        return;
    }
    if (canbyOne(px,py,cx,cy)){
        drawbyOne(px,py,cx,cy);
        return;
    }
    if (canbyTwo(px,py,cx,cy)){
        drawbyTwo(px,py,cx,cy);
        return;
    }
}
void MainWindow::sleep(int time)
{
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(time, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//通过执行事件循环对象的exec函数来阻塞程序，直到定时器超时并触发退出，才会继续执行后续的代码
}
void MainWindow::drawLine_adjacent(int px, int py, int cx, int cy)
{
    if((px==cx&&abs(py-cy)==1)||(py==cy&&abs(px-cx)==1)){
        if(px==cx&&abs(py-cy)==1){//一上一下
            if(map[px+1][py]==0&&map_prop[px+1][py]==0&&map[cx+1][cy]==0&&map_prop[cx+1][cy]==0){//右侧可连线
                if(py<cy){//前一个在当前上方
                    map[px+1][py]=1004;
                    map[cx+1][cy]=1005;
                    sleep(80);
                    map[px+1][py]=0;
                    map[cx+1][cy]=0;
                    return;
                }
                if(py>cy){//前一个在当前下方
                    map[px+1][py]=1005;
                    map[cx+1][cy]=1004;
                    sleep(80);
                    map[px+1][py]=0;
                    map[cx+1][cy]=0;
                    return;
                }
            }
            if(map[px-1][py]==0&&map_prop[px-1][py]==0&&map[cx-1][cy]==0&&map_prop[cx-1][cy]==0){//左侧可连线
                if(py<cy){//前一个在当前上方
                    map[px-1][py]=1003;
                    map[cx-1][cy]=1002;
                    sleep(80);
                    map[px-1][py]=0;
                    map[cx-1][cy]=0;
                    return;
                }
                if(py>cy){//前一个在当前下方
                    map[px-1][py]=1002;
                    map[cx-1][cy]=1003;
                    sleep(80);
                    map[px-1][py]=0;
                    map[cx-1][cy]=0;
                    return;
                }
            }
        }
        if(py==cy&&abs(px-cx)==1){//一左一右
            if(map[px][py+1]==0&&map_prop[px][py+1]==0&&map[cx][cy+1]==0&&map_prop[cx][cy+1]==0){//下方可连线
                if(px<cx){//前一个在当前左侧
                    map[px][py+1]=1002;
                    map[cx][cy+1]=1005;
                    sleep(80);
                    map[px][py+1]=0;
                    map[cx][cy+1]=0;
                    return;
                }
                if(px>cx){//前一个在当前有侧
                    map[px][py+1]=1005;
                    map[cx][cy+1]=1002;
                    sleep(80);
                    map[px][py+1]=0;
                    map[cx][cy+1]=0;
                    return;
                }
            }
            if(map[px][py-1]==0&&map_prop[px][py-1]==0&&map[cx][cy-1]==0&&map_prop[cx][cy-1]==0){//上方可连线
                if(px<cx){//前一个在当前左侧
                    map[px][py-1]=1003;
                    map[cx][cy-1]=1004;
                    sleep(80);
                    map[px][py-1]=0;
                    map[cx][cy-1]=0;
                    return;
                }
                if(px>cx){//前一个在当前有侧
                    map[px][py-1]=1004;
                    map[cx][cy-1]=1003;
                    sleep(80);
                    map[px][py-1]=0;
                    map[cx][cy-1]=0;
                    return;
                }
            }
        }
    }
}
void MainWindow::drawLine(int px, int py, int cx, int cy, bool type)
{
    if (px==cx){//同一竖直线上
        if (py>cy)
            std::swap(py,cy);//确保前一点在当前点上方
        switch (type) {
        case 1:
            for (int i=py+1; i<cy; i++){
                map[px][i] = 1000;//画线
            }
            break;
        case 0:
            for (int i=py+1; i<cy; i++){
                map[px][i] = 0;//擦线
            }
            break;
        }

    }
    else if (py==cy){//同一水平线上
        if (px>cx)
            std::swap(px,cx);//确保前一点在当前点左侧
        switch (type) {
        case 1:
            for (int i=px+1; i<cx; i++){
                map[i][py] = 1001;//画线
            }
            break;
        case 0:
            for (int i=px+1; i<cx; i++){
                map[i][py] = 0;//擦线
            }
            break;
        }
    }
}
void MainWindow::drawbyOne(int px, int py, int cx, int cy)
{
    if(px>cx){
        std::swap(px,cx);
        std::swap(py,cy);
    }//使前一点位于当前点左侧
    if(py<cy){//前一点在当前点左上方
        if(cx-px==1&&cy-py==1){
            if((map[px][cy]==0&&map_prop[px][cy]==0)||(map[cx][py]==0&&map_prop[cx][py]==0)){
                if(map[px][cy]==0&&map_prop[px][cy]==0){
                    map[px][cy]=1002;
                    sleep(80);
                    map[px][cy]=0;
                    return;
                }
                if(map[cx][py]==0&&map_prop[cx][py]==0){
                    map[cx][py]=1004;
                    sleep(80);
                    map[cx][py]=0;
                    return;
                }
            }
        }
        if(cx-px==1&&cy-py!=1){
            if((map[px][cy]==0&&map_prop[px][cy]==0&&isBlank_line(px,py,px,cy))||(map[cx][py]==0&&map_prop[cx][py]==0&&isBlank_line(cx,py,cx,cy))){
                if(map[px][cy]==0&&map_prop[px][cy]==0&&isBlank_line(px,py,px,cy)){
                    map[px][cy]=1002;
                    drawLine(px,py,px,cy,1);
                    sleep(80);
                    map[px][cy]=0;
                    drawLine(px,py,px,cy,0);
                    return;
                }
                if(map[cx][py]==0&&map_prop[cx][py]==0&&isBlank_line(cx,py,cx,cy)){
                    map[cx][py]=1004;
                    drawLine(cx,py,cx,cy,1);
                    sleep(80);
                    map[cx][py]=0;
                    drawLine(cx,py,cx,cy,0);
                    return;
                }
            }
        }
        if(cy-py==1&&cx-py!=1){
            if((map[px][cy]==0&&map_prop[px][cy]==0&&isBlank_row(px,cy,cx,cy))||(map[cx][py]==0&&map_prop[cx][py]==0&&isBlank_row(px,py,cx,py))){
                if(map[px][cy]==0&&map_prop[px][cy]==0&&isBlank_row(px,cy,cx,cy)){
                    map[px][cy]=1002;
                    drawLine(px,cy,cx,cy,1);
                    sleep(80);
                    map[px][cy]=0;
                    drawLine(px,cy,cx,cy,0);
                    return;
                }
                if(map[cx][py]==0&&map_prop[cx][py]==0&&isBlank_row(px,py,cx,py)){
                    map[cx][py]=1004;
                    drawLine(px,py,cx,py,1);
                    sleep(80);
                    map[cx][py]=0;
                    drawLine(px,py,cx,py,0);
                    return;
                }
            }
        }
        else {
            if(isBlank_row(px,py,cx,py)&&isBlank_line(cx,py,cx,cy)&&map[cx][py]==0&&map_prop[cx][py]==0){//→↓
                drawLine(px,py,cx,py,1);
                drawLine(cx,py,cx,cy,1);
                map[cx][py]=1004;
                sleep(80);
                drawLine(px,py,cx,py,0);
                drawLine(cx,py,cx,cy,0);
                map[cx][py]=0;
                return;
            }
            if(isBlank_line(px,py,px,cy)&&isBlank_row(px,cy,cx,cy)&&map[px][cy]==0&&map_prop[px][cy]==0){//↓→
                drawLine(px,py,px,cy,1);
                drawLine(px,cy,cx,cy,1);
                map[px][cy]=1002;
                sleep(80);
                drawLine(px,py,px,cy,0);
                drawLine(px,cy,cx,cy,0);
                map[px][cy]=0;
                return;
            }
        }
    }
    if(py>cy){//前一点在当前点左下方
        if(cx-px==1&&py-cy==1){
            if((map[px][cy]==0&&map_prop[px][cy]==0)||(map[cx][py]==0&&map_prop[cx][py]==0)){
                if(map[px][cy]==0&&map_prop[px][cy]==0){
                    map[px][cy]=1003;
                    sleep(80);
                    map[px][cy]=0;
                    return;
                }
                if(map[cx][py]==0&&map_prop[cx][py]==0){
                    map[cx][py]=1005;
                    sleep(80);
                    map[cx][py]=0;
                    return;
                }
            }
        }
        if(cx-px==1&&py-cy!=1){
            if((map[px][cy]==0&&map_prop[px][cy]==0&&isBlank_line(px,py,px,cy))||(map[cx][py]==0&&map_prop[cx][py]==0&&isBlank_line(cx,py,cx,cy))){
                if(map[px][cy]==0&&map_prop[px][cy]==0&&isBlank_line(px,py,px,cy)){
                    map[px][cy]=1003;
                    drawLine(px,py,px,cy,1);
                    sleep(80);
                    map[px][cy]=0;
                    drawLine(px,py,px,cy,0);
                    return;
                }
                if(map[cx][py]==0&&map_prop[cx][py]==0&&isBlank_line(cx,py,cx,cy)){
                    map[cx][py]=1005;
                    drawLine(cx,py,cx,cy,1);
                    sleep(80);
                    map[cx][py]=0;
                    drawLine(cx,py,cx,cy,0);
                    return;
                }
            }
        }
        if(py-cy==1&&cx-px!=1){
            if((map[px][cy]==0&&map_prop[px][cy]==0&&isBlank_row(px,cy,cx,cy))||(map[cx][py]==0&&map_prop[cx][py]==0&&isBlank_row(px,py,cx,py))){
                if(map[px][cy]==0&&map_prop[px][cy]==0&&isBlank_row(px,cy,cx,cy)){
                    map[px][cy]=1003;
                    drawLine(px,cy,cx,cy,1);
                    sleep(80);
                    map[px][cy]=0;
                    drawLine(px,cy,cx,cy,0);
                    return;
                }
                if(map[cx][py]==0&&map_prop[cx][py]==0&&isBlank_row(px,py,cx,py)){
                    map[cx][py]=1005;
                    drawLine(px,py,cx,py,1);
                    sleep(80);
                    map[cx][py]=0;
                    drawLine(px,py,cx,py,0);
                    return;
                }
            }
        }
        else {
            if(isBlank_line(px,py,px,cy)&&isBlank_row(px,cy,cx,cy)&&map[px][cy]==0&&map_prop[px][cy]==0){//↑→
                drawLine(px,py,px,cy,1);
                drawLine(px,cy,cx,cy,1);
                map[px][cy]=1003;
                sleep(80);
                drawLine(px,py,px,cy,0);
                drawLine(px,cy,cx,cy,0);
                map[px][cy]=0;
                return;
            }
            if(isBlank_row(px,py,cx,py)&&isBlank_line(cx,py,cx,cy)&&map[cx][py]==0&&map_prop[cx][py]==0){//→↑
                drawLine(px,py,cx,py,1);
                drawLine(cx,py,cx,cy,1);
                map[cx][py]=1005;
                sleep(80);
                drawLine(px,py,cx,py,0);
                drawLine(cx,py,cx,cy,0);
                map[cx][py]=0;
                return;
            }
        }
    }
}
void MainWindow::drawbyTwo(int px, int py, int cx, int cy)
{
    if(px>cx){
        std::swap(px,cx);
        std::swap(py,cy);
    }//使前一点位于当前点左侧
    for (int i=0; i<mapBox; i++){
        if (i==py || i==cy)
            continue;
        bool b = true;//找到一条不同于fy与cy的水平线
        for (int j=px; j<=cx; j++){
            if (map[j][i]!=0||map_prop[j][i]!=0){
                b = false;//水平线上有方块元素值不为0
            }
        }
        if (b)//水平线上方块元素值全为0（全空）
            if (isBlank_line(px,py,px,i) && isBlank_line(cx,cy,cx,i)){//两条竖直线
                if(py<cy){//前一点在当前点左上方
                    if(i<py){//水平线在2点之上
                        if(py-i>1){
                            drawLine(px,py,px,i,1);
                        }
                        map[px][i]=1003;
                        drawLine(px,i,cx,i,1);
                        drawLine(cx,cy,cx,i,1);
                        map[cx][i]=1004;
                        sleep(80);
                        if(py-i>1){
                            drawLine(px,py,px,i,0);
                        }
                        map[px][i]=0;
                        drawLine(px,i,cx,i,0);
                        drawLine(cx,cy,cx,i,0);
                        map[cx][i]=0;
                        return;
                    }
                    if(py<i&&i<cy){//水平线在2点之间
                        if(i-py>1){
                            drawLine(px,py,px,i,1);
                        }
                        map[px][i]=1002;
                        drawLine(px,i,cx,i,1);
                        if(cy-i>1){
                            drawLine(cx,cy,cx,i,1);
                        }
                        map[cx][i]=1004;
                        sleep(80);
                        if(i-py>1){
                            drawLine(px,py,px,i,0);
                        }
                        map[px][i]=0;
                        drawLine(px,i,cx,i,0);
                        if(cy-i>1){
                            drawLine(cx,cy,cx,i,0);
                        }
                        map[cx][i]=0;
                        return;
                    }
                    if(i>cy){//水平线在2点之下
                        drawLine(px,py,px,i,1);
                        map[px][i]=1002;
                        drawLine(px,i,cx,i,1);
                        if(i-cy>1){
                            drawLine(cx,cy,cx,i,1);
                        }
                        map[cx][i]=1005;
                        sleep(80);
                        drawLine(px,py,px,i,0);
                        map[px][i]=0;
                        drawLine(px,i,cx,i,0);
                        if(i-cy>1){
                            drawLine(cx,cy,cx,i,0);
                        }
                        map[cx][i]=0;
                        return;
                    }
                }
                if(py>cy){//前一点在当前点左下方
                    if(i<cy){//水平线在2点之上
                        drawLine(px,py,px,i,1);
                        map[px][i]=1003;
                        drawLine(px,i,cx,i,1);
                        if(cy-i>1){
                            drawLine(cx,cy,cx,i,1);
                        }
                        map[cx][i]=1004;
                        sleep(80);
                        drawLine(px,py,px,i,0);
                        map[px][i]=0;
                        drawLine(px,i,cx,i,0);
                        if(cy-i>1){
                            drawLine(cx,cy,cx,i,0);
                        }
                        map[cx][i]=0;
                        return;
                    }
                    if(cy<i&&i<py){//水平线在2点之间
                        if(py-i>1){
                            drawLine(px,py,px,i,1);
                        }
                        map[px][i]=1003;
                        drawLine(px,i,cx,i,1);
                        if(i-cy>1){
                            drawLine(cx,cy,cx,i,1);
                        }
                        map[cx][i]=1005;
                        sleep(80);
                        if(py-i>1){
                            drawLine(px,py,px,i,0);
                        }
                        map[px][i]=0;
                        drawLine(px,i,cx,i,0);
                        if(i-cy>1){
                            drawLine(cx,cy,cx,i,0);
                        }
                        map[cx][i]=0;
                        return;
                    }
                    if(i>py){//水平线在2点之下
                        if(i-py>1){
                            drawLine(px,py,px,i,1);
                        }
                        map[px][i]=1002;
                        drawLine(px,i,cx,i,1);
                        drawLine(cx,cy,cx,i,1);
                        map[cx][i]=1005;
                        sleep(80);
                        if(i-py>1){
                            drawLine(px,py,px,i,0);
                        }
                        map[px][i]=0;
                        drawLine(px,i,cx,i,0);
                        drawLine(cx,cy,cx,i,0);
                        map[cx][i]=0;
                        return;
                    }
                }
                if(py==cy){//前一点在当前点水平左侧
                    if(i<py){//水平线在2点上方
                        if(py-i>1){
                            drawLine(px,py,px,i,1);
                            drawLine(cx,cy,cx,i,1);
                        }
                        map[px][i]=1003;
                        drawLine(px,i,cx,i,1);
                        map[cx][i]=1004;
                        sleep(80);
                        if(py-i>1){
                            drawLine(px,py,px,i,0);
                            drawLine(cx,cy,cx,i,0);
                        }
                        map[px][i]=0;
                        drawLine(px,i,cx,i,0);
                        map[cx][i]=0;
                        return;
                    }
                    if(i>py){//水平线在2点下方
                        if(i-py>1){
                            drawLine(px,py,px,i,1);
                            drawLine(cx,cy,cx,i,1);
                        }
                        map[px][i]=1002;
                        drawLine(px,i,cx,i,1);
                        map[cx][i]=1005;
                        sleep(80);
                        if(i-py>1){
                            drawLine(px,py,px,i,0);
                            drawLine(cx,cy,cx,i,0);
                        }
                        map[px][i]=0;
                        drawLine(px,i,cx,i,0);
                        map[cx][i]=0;
                        return;
                    }
                }
            }
    }
    if(py>cy){
        std::swap(px,cx);
        std::swap(py,cy);
    }//使前一点位于当前点上方
    for (int i=0; i<mapBox; i++){
        if (i==px || i==cx)
            continue;
        bool b = true;//找到一条不同于px与cx的垂直线
        for (int j=py; j<=cy; j++){
            if (map[i][j]!=0||map_prop[i][j]!=0){
                b = false;//垂直线上有方块元素值不为0
            }
        }
        if (b)//垂直线上方块元素值全为0（全空）
            if (isBlank_row(px,py,i,py) && isBlank_row(cx,cy,i,cy)){//两条水平线
                if(px<cx){//前一点在当前点左上方
                    if(i<px){//垂直线在2点左侧
                        if(px-i>1){
                            drawLine(px,py,i,py,1);
                        }
                        map[i][py]=1003;
                        drawLine(i,py,i,cy,1);
                        drawLine(cx,cy,i,cy,1);
                        map[i][cy]=1002;
                        sleep(80);
                        if(px-i>1){
                            drawLine(px,py,i,py,0);
                        }
                        map[i][py]=0;
                        drawLine(i,py,i,cy,0);
                        drawLine(cx,cy,i,cy,0);
                        map[i][cy]=0;
                        return;
                    }
                    if(px<i&&i<cx){//垂直线在2点中间
                        if(i-px>1){
                            drawLine(px,py,i,py,1);
                        }
                        map[i][py]=1004;
                        drawLine(i,py,i,cy,1);
                        if(cx-i>1){
                            drawLine(cx,cy,i,cy,1);
                        }
                        map[i][cy]=1002;
                        sleep(80);
                        if(i-px>1){
                            drawLine(px,py,i,py,0);
                        }
                        map[i][py]=0;
                        drawLine(i,py,i,cy,0);
                        if(cx-i>1){
                            drawLine(cx,cy,i,cy,0);
                        }
                        map[i][cy]=0;
                        return;
                    }
                    if(cx<i){//垂直线在2点右侧
                        drawLine(px,py,i,py,1);
                        map[i][py]=1004;
                        drawLine(i,py,i,cy,1);
                        if(i-cx>1){
                            drawLine(cx,cy,i,cy,1);
                        }
                        map[i][cy]=1005;
                        sleep(80);
                        drawLine(px,py,i,py,0);
                        map[i][py]=0;
                        drawLine(i,py,i,cy,0);
                        if(i-cx>1){
                            drawLine(cx,cy,i,cy,0);
                        }
                        map[i][cy]=0;
                        return;
                    }
                }
                if(px>cx){//前一点在当前点右上方
                    if(i<cx){//垂直线在2点左侧
                        drawLine(px,py,i,py,1);
                        map[i][py]=1003;
                        drawLine(i,py,i,cy,1);
                        if(cx-i>1){
                            drawLine(cx,cy,i,cy,1);
                        }
                        map[i][cy]=1002;
                        sleep(80);
                        drawLine(px,py,i,py,0);
                        map[i][py]=0;
                        drawLine(i,py,i,cy,0);
                        if(cx-i>1){
                            drawLine(cx,cy,i,cy,0);
                        }
                        map[i][cy]=0;
                        return;
                    }
                    if(cx<i&&i<px){//垂直线在2点中间
                        if(px-i>1){
                            drawLine(px,py,i,py,1);
                        }
                        map[i][py]=1003;
                        drawLine(i,py,i,cy,1);
                        if(i-cx>1){
                            drawLine(cx,cy,i,cy,1);
                        }
                        map[i][cy]=1005;
                        sleep(80);
                        if(px-i>1){
                            drawLine(px,py,i,py,0);
                        }
                        map[i][py]=0;
                        drawLine(i,py,i,cy,0);
                        if(i-cx>1){
                            drawLine(cx,cy,i,cy,0);
                        }
                        map[i][cy]=0;
                        return;
                    }
                    if(px<i){//垂直线在2点右侧
                        if(i-px>1){
                            drawLine(px,py,i,py,1);
                        }
                        map[i][py]=1004;
                        drawLine(i,py,i,cy,1);
                        drawLine(cx,cy,i,cy,1);
                        map[i][cy]=1005;
                        sleep(80);
                        if(i-px>1){
                            drawLine(px,py,i,py,0);
                        }
                        map[i][py]=0;
                        drawLine(i,py,i,cy,0);
                        drawLine(cx,cy,i,cy,0);
                        map[i][cy]=0;
                        return;
                    }
                }
                if(px==cx){//前一点在当前点正上方
                    if(i<px){//垂直线在2点左侧
                        if(px-i>1){
                            drawLine(px,py,i,py,1);
                            drawLine(cx,cy,i,cy,1);
                        }
                        map[i][py]=1003;
                        drawLine(i,py,i,cy,1);
                        map[i][cy]=1002;
                        sleep(80);
                        if(px-i>1){
                            drawLine(px,py,i,py,0);
                            drawLine(cx,cy,i,cy,0);
                        }
                        map[i][py]=0;
                        drawLine(i,py,i,cy,0);
                        map[i][cy]=0;
                        return;
                    }
                    if(i>px){//垂直线在2点右侧
                        if(i-px>1){
                            drawLine(px,py,i,py,1);
                            drawLine(cx,cy,i,cy,1);
                        }
                        map[i][py]=1004;
                        drawLine(i,py,i,cy,1);
                        map[i][cy]=1005;
                        sleep(80);
                        if(i-px>1){
                            drawLine(px,py,i,py,0);
                            drawLine(cx,cy,i,cy,0);
                        }
                        map[i][py]=0;
                        drawLine(i,py,i,cy,0);
                        map[i][cy]=0;
                        return;
                    }
                }
            }
    }
}
struct Point {
    int x;
    int y;
};
void MainWindow::gameisOver() {
    bool isOver = 1;//默认游戏已经不能再连了
    std::vector<Point> nonEmptyPoints;
    // 找出所有非空格子的位置
    for (int i = 1; i < mapBox - 1; i++) {
        for (int j = 1; j < mapBox - 1; j++) {
            if (map[i][j] != 0) {
                nonEmptyPoints.push_back({i, j});
            }
        }
    }
    // 遍历非空格子，查找是否有可以连接的格子
    for (int i = 0; i < nonEmptyPoints.size(); i++) {
        Point p1 = nonEmptyPoints[i];
        for (int j = i + 1; j < nonEmptyPoints.size(); j++) {
            Point p2 = nonEmptyPoints[j];
            if (canLink(p1.x, p1.y, p2.x, p2.y)) {
                isOver = false;
                break;
            }
        }
    }

    if (isOver) {
        endDialog();
    }
}
void MainWindow::endDialog()
{
    switch (mode) {
    case 1:{
        timer->stop();
        QMessageBox::StandardButton result =QMessageBox::question(this, "游戏结束",QString::asprintf("你已完成所有连连看!\n你的得分是:%d\n要再来一局吗？",player1.score),QMessageBox::Yes | QMessageBox::No);
        if(result==QMessageBox::No)
            exit(0);
        else    startanewGame();
        break;
    }
    case 2:{
        timer->stop();
        if(player1.score>player2.score){
            QMessageBox::StandardButton result =QMessageBox::question(this, "游戏结束",QString::asprintf("玩家1取胜!\n玩家1的得分是:%d\n玩家2的得分是:%d\n要再来一局吗？",player1.score,player2.score),QMessageBox::Yes | QMessageBox::No);
            if(result==QMessageBox::No)
                exit(0);
            else    startanewGame();
        }
        if(player1.score<player2.score){
            QMessageBox::StandardButton result =QMessageBox::question(this, "游戏结束",QString::asprintf("玩家2取胜!\n玩家1的得分是:%d\n玩家2的得分是:%d\n要再来一局吗？",player1.score,player2.score),QMessageBox::Yes | QMessageBox::No);
            if(result==QMessageBox::No)
                exit(0);
            else    startanewGame();
        }
        if(player1.score==player2.score){
            QMessageBox::StandardButton result =QMessageBox::question(this, "游戏结束",QString::asprintf("平局!\n玩家1的得分是:%d\n玩家2的得分是:%d\n要再来一局吗？",player1.score,player2.score),QMessageBox::Yes | QMessageBox::No);
            if(result==QMessageBox::No)
                exit(0);
            else    startanewGame();
        }
        break;
    }
    }
}
void MainWindow::plusTime()
{
    int currentValue=ui->timeBar->value();
    if(currentValue>=gameTime-30000){
        ui->timeBar->setValue(gameTime);
    }
    else{
        ui->timeBar->setValue(currentValue+30000);
    }
    if (ui->timeBar->value() <= gameTime / 10){
        ui->timeBar->setStyleSheet("QProgressBar::chunk{background:red}");
    }
    if(ui->timeBar->value()>gameTime/10){
        ui->timeBar->setStyleSheet("QProgressBar::chunk{background:green}");
    }
}
void MainWindow::shuffle()
{
    player1.playerhaveselectedOne=0;
    player2.playerhaveselectedOne=0;
    // 将地图上的道具保存到一个数组中
    const int propsSize = (mapBox-2) * (mapBox-2);
    int props[propsSize];
    int propCount = 0;
    for (int i = 1; i < mapBox-1; ++i) {
        for (int j = 1; j < mapBox-1; ++j) {
            if (map_prop[i][j] !=0) {
                props[propCount++] = map_prop[i][j];
                map_prop[i][j] = 0;
            } else {
                props[propCount++] = map[i][j]%10;
            }
        }
    }
    // Fisher-Yates随机算法对数组进行重排
    srand(time(NULL));
    for (int i = propsSize - 1; i >= 1; --i) {
        int j = rand() % (i + 1);
        std::swap(props[i], props[j]);
    }
    // 将重排后的结果填充回地图
    int propIndex = 0;
    for (int i = 1; i < mapBox-1; ++i) {
        for (int j = 1; j < mapBox-1; ++j) {
            if (props[propIndex] >= 10000) {
                map_prop[i][j] = props[propIndex];
                map[i][j] = 0;
            } else {
                map[i][j] = props[propIndex];
            }
            ++propIndex;
        }
    }
}
void MainWindow::launchHint()
{
    if(playercanbeHelped){
        hint();
    }
}
//取消提示
void MainWindow::eliminateHint()
{
    for(int i=1;i<mapBox-1;i++){
        for(int j=1;j<mapBox-1;j++){
            if(map[i][j]>20&&map[i][j]<30)
                map[i][j]%=10;
        }
    }
}
void MainWindow::hint()
{
    std::vector<Point> nonEmptyPoints;
    // 找出所有非空格子的位置
    for (int i = 1; i < mapBox - 1; i++) {
        for (int j = 1; j < mapBox - 1; j++) {
            if (map[i][j] != 0) {
                nonEmptyPoints.push_back({i, j});
            }
        }
    }
    // 遍历非空格子，查找是否有可以连接的格子
    for (int i = 0; i < nonEmptyPoints.size(); i++) {
        Point p1 = nonEmptyPoints[i];
        for (int j = i + 1; j < nonEmptyPoints.size(); j++) {
            Point p2 = nonEmptyPoints[j];
            if (canLink(p1.x, p1.y, p2.x, p2.y)&&(map[p1.x][p1.y]>0&&map[p1.x][p1.y]<10)&&(map[p2.x][p2.y]>0&&map[p2.x][p2.y]<10)) {
                map[p1.x][p1.y]+=20;
                map[p2.x][p2.y]+=20;
                return;
            }
            /*
                if (canLink(p1.x, p1.y, p2.x, p2.y)&&(map[p1.x][p1.y]>0&&map[p1.x][p1.y]<10)&&(map[p2.x][p2.y]>10&&map[p2.x][p2.y]<20)) {
                    map[p1.x][p1.y]+=20;
                    //                map[p2.x][p2.y]+=10;
                    return;
                }
                if (canLink(p1.x, p1.y, p2.x, p2.y)&&(map[p1.x][p1.y]>10&&map[p1.x][p1.y]<20)&&(map[p2.x][p2.y]>0&&map[p2.x][p2.y]<10)) {
                    //                map[p1.x][p1.y]+=10;
                    map[p2.x][p2.y]+=20;
                    return;
                }
                if (canLink(p1.x, p1.y, p2.x, p2.y)&&(map[p1.x][p1.y]>0&&map[p1.x][p1.y]<10)&&(map[p2.x][p2.y]>100&&map[p2.x][p2.y]<110)) {
                    map[p1.x][p1.y]+=20;
                    //                map[p2.x][p2.y]-=80;
                    return;
                }
                if (canLink(p1.x, p1.y, p2.x, p2.y)&&(map[p1.x][p1.y]>100&&map[p1.x][p1.y]<110)&&(map[p2.x][p2.y]>0&&map[p2.x][p2.y]<10)) {
                    //                map[p1.x][p1.y]-=80;
                    map[p2.x][p2.y]+=20;
                    return;
                }
                if (canLink(p1.x, p1.y, p2.x, p2.y)&&(map[p1.x][p1.y]>20&&map[p1.x][p1.y]<30)&&(map[p2.x][p2.y]>20&&map[p2.x][p2.y]<30)) {
                    return;
                }
                */
        }
    }
}
void MainWindow::dropProp()
{
    /*
                                            // 随机概率引擎
                                            std::random_device rd;
                                            std::mt19937 gen(rd());
                                            std::uniform_int_distribution<int> dist(1, 100); // 区间 [1, 100]
                                            int p = dist(gen); // 生成随机概率
                                            */
    //        srand((unsigned)time(NULL));
    int p= rand()%10000;//随机概率
    //        qDebug()<<p<<endl;
    if(p<=7){
        bool b = false;
        int i,j;
        while (!b){
            i = rand()%mapBox;
            j = rand()%mapBox;
            if (map[i][j] == 0&&map_prop[i][j]==0)
                b = true;//找到空位

        }
        int help;
        switch (mode) {
        case 1:
            help = rand()%3;
            break;
        case 2:
            help = rand()%5;
            break;
        }
        map_prop[i][j] = 10000+help;
        //随机道具
    }
}
bool MainWindow::canHint()
{
    for (int i=1; i<mapBox-1; i++){
        for (int j=1; j<mapBox-1; j++){
            if (map[i][j]>20&&map[i][j]<30){
                return false;
            }
        }
    }
    return true;
}


MainWindow::~MainWindow()
{
    delete ui;
}

