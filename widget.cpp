#include "widget.h"
#include "ui_widget.h"
#include <QPainter>
#include <qrandom.h>
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QAudioOutput>
#include <QMediaPlayer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //使用定时器
    timer = new QTimer();//动态内存匹配
    //定时器有timeout信号
    //信号槽（信号和对应函数关联）
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    resize(2400,1600);

    //初始化蛇身
    QRectF rect(1600,800,nodeWidth,nodeHeight);
    snake.append(rect);
    addUp();
    addUp();

    //第二条蛇身的初始化
    QRectF rect2(800,800,nodeWidth,nodeHeight);
    snake2.append(rect2);
    addUp2();
    addUp2();

    //初始化奖品随机出现
     addNewReward();

    //初始化墙的出现
     addborder();

     //背景音乐处理
     //播放音乐，加载模块
     //一个媒体播放对象

     mediaPlayer = new QMediaPlayer(this);
     // 连接到媒体播放器的状态变化信号（可选）
     //创建好播放器
     connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status){
             if (status == QMediaPlayer::EndOfMedia) {
                 // 播放结束后重新播放
                 mediaPlayer->play();
             }
         });
     //获取当前媒体的时长，通过信号与槽函数获取
     connect(mediaPlayer,&QMediaPlayer::durationChanged,this,[=](qint64 duration)
     {
         ui->totallable->setText(QString("%1:%2").arg(duration / 1000 / 60,2,10,QChar('0')).arg(duration % (1000 * 60) / 1000, 2, 10, QChar('0')));
         ui->playCourseSlider->setRange(0,duration);
     });
     //获取当前播放时长
     connect(mediaPlayer,&QMediaPlayer::positionChanged,this,[=](qint64 pos)
     {
         ui->curlable->setText(QString("%1:%2").arg(pos / 1000 / 60,2,10,QChar('0')).arg(pos % (1000 * 60) / 1000, 2, 10, QChar('0')));
         ui->playCourseSlider->setValue(pos);
     });
    //拖动滑块，进度改变
    connect(ui->playCourseSlider,&QSlider::sliderMoved,mediaPlayer,&QMediaPlayer::setPosition);
    //音量设置
    connect(ui->volumeSlider, &QSlider::valueChanged, this, [this](int value){
    // 将滑块的值映射到音量范围（0-100）
        mediaPlayer->setVolume(value);
    });
    // 设置滑块的范围和初始值
    ui->volumeSlider->setRange(0, 100);
    ui->volumeSlider->setValue(50); // 初始设置为 50% 音量
}

Widget::~Widget()
{
    delete ui;
}
//控制方向
void Widget::keyPressEvent(QKeyEvent *event){
    switch(event->key()){
    case Qt::Key_I:
        if(moveFlag!=DIR_DOWN){
            moveFlag=DIR_UP;
        }
        break;
    case Qt::Key_K:
        if(moveFlag!=DIR_UP){
            moveFlag=DIR_DOWN;
        }
        break;
    case Qt::Key_L:
        if(moveFlag!=DIR_LEFT){
            moveFlag=DIR_RIGHT;
        }
        break;
    case Qt::Key_J:
        if(moveFlag!=DIR_RIGHT){
            moveFlag=DIR_LEFT;
        }
        break;

        //第二条蛇的移动
    case Qt::Key_W:
        if(moveFlag2!=DIR_DOWN2){
            moveFlag2=DIR_UP2;
        }
        break;
    case Qt::Key_S:
        if(moveFlag2!=DIR_UP2){
            moveFlag2=DIR_DOWN2;
        }
        break;
    case Qt::Key_D:
        if(moveFlag2!=DIR_LEFT2){
            moveFlag2=DIR_RIGHT2;
        }
        break;
    case Qt::Key_A:
        if(moveFlag2!=DIR_RIGHT2){
            moveFlag2=DIR_LEFT2;
        }
        break;

    case Qt::Key_G:
        if(gamestart==false){
            gamestart=true;
            //启动定时器
            timer->start(time);
        }
        else{
            gamestart=false;
            timer->stop();
        }
        break;

    case Qt::Key_Shift:
           if (!event->isAutoRepeat()&&!dPressed) { // 检查按键是否重复，以及是否已经处理过按键 shift 的事件
               time=75;
               // 停止定时器
               timer->stop();
               // 更新定时器时间间隔
               timer->setInterval(time);
               // 断开之前的连接
               disconnect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
               // 连接定时器的超时信号和槽函数
               connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
               // 重新启动定时器
               timer->start();
               dPressed = true; // 标记按键 shift 已经处理过
           }
           break;
        break;
    default:
        break;
    }
}

void Widget::keyReleaseEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Shift&&dPressed) {
            time=150;
            // 停止定时器
            timer->stop();
            // 更新定时器时间间隔
            timer->setInterval(time);
            // 断开之前的连接
            disconnect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
            // 连接定时器的超时信号和槽函数
            connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
            // 重新启动定时器
            timer->start();
        dPressed = false; // 释放按键 shift 时取消标记
    }
}


void Widget::timeout(){
    int count=1;
    //判断有没有重合
   if(snake[0].intersects(rewardNode)){
        count++;
        addNewReward();
        addborder();
    }

    while(count--){
    switch(moveFlag){
    case DIR_UP:
        //在顶部加一个小方块
        addUp();
        break;
    case DIR_DOWN:
        addDown();
        break;
    case DIR_LEFT:
        addleft();
        break;
    case DIR_RIGHT:
        addRight();
        break;
    default:
        break;
    }
    }
    //队尾减一个方块
    deleteLast();
    update();//更新，painter重新绘制这个队列

    //第二条蛇的timeout函数
    int count2=1;
    //判断有没有重合
   if(snake2[0].intersects(rewardNode)){
        count2++;
        addNewReward();
        addborder();
    }

    while(count2--){
    switch(moveFlag2){
    case DIR_UP2:
        //在顶部加一个小方块
        addUp2();
        break;
    case DIR_DOWN2:
        addDown2();
        break;
    case DIR_LEFT2:
        addleft2();
        break;
    case DIR_RIGHT2:
        addRight2();
        break;
    default:
        break;
    }
    }
    //队尾减一个方块
    deleteLast2();
    update();//更新，painter重新绘制这个队列
}

void Widget::addUp(){
    QPointF leftTop;
    QPointF rightBottom;

    if(snake[0].y()-nodeHeight<0){
        leftTop=QPointF(snake[0].x(),this->height()-nodeHeight);
        rightBottom=QPointF(snake[0].x()+nodeWidth,this->height());
    }
    else{
    leftTop=QPointF(snake[0].x(),snake[0].y()-nodeHeight);
    rightBottom=snake[0].topRight();
    }
    snake.insert(0,QRectF(leftTop,rightBottom));

}

void Widget::addUp2(){
    QPointF leftTop2;
    QPointF rightBottom2;

    if(snake2[0].y()-nodeHeight<0){
        leftTop2=QPointF(snake2[0].x(),this->height()-nodeHeight);
        rightBottom2=QPointF(snake2[0].x()+nodeWidth,this->height());
    }
    else{
    leftTop2=QPointF(snake2[0].x(),snake2[0].y()-nodeHeight);
    rightBottom2=snake2[0].topRight();
    }
    snake2.insert(0,QRectF(leftTop2,rightBottom2));

}

void Widget::addDown(){
    QPointF leftTop;
    QPointF rightBottom;

    if(snake[0].y()+2*nodeHeight>this->height()){
        leftTop=QPointF(snake[0].x(),0);
        rightBottom=QPointF(snake[0].x()+nodeWidth,nodeHeight);
    }
    else{
    leftTop=snake[0].bottomLeft();
    rightBottom=QPointF(snake[0].x()+nodeWidth,snake[0].y()+2*nodeHeight);
    }
    snake.insert(0,QRectF(leftTop,rightBottom));
}

void Widget::addDown2(){
    QPointF leftTop2;
    QPointF rightBottom2;

    if(snake2[0].y()+2*nodeHeight>this->height()){
        leftTop2=QPointF(snake2[0].x(),0);
        rightBottom2=QPointF(snake2[0].x()+nodeWidth,nodeHeight);
    }
    else{
    leftTop2=snake2[0].bottomLeft();
    rightBottom2=QPointF(snake2[0].x()+nodeWidth,snake2[0].y()+2*nodeHeight);
    }
    snake2.insert(0,QRectF(leftTop2,rightBottom2));
}

void Widget::addleft(){
    QPointF leftTop;
    QPointF rightBottom;

    if(snake[0].x()-nodeWidth<0){
        leftTop=QPointF(this->width()-nodeWidth,snake[0].y());
        rightBottom=QPointF(this->width(),snake[0].y()+nodeHeight);
    }
    else{
        leftTop=snake[0].topLeft()-QPointF(nodeWidth,0);
        rightBottom=snake[0].bottomLeft();
    }
     snake.insert(0,QRectF(leftTop,rightBottom));
}

void Widget::addleft2(){
    QPointF leftTop2;
    QPointF rightBottom2;

    if(snake2[0].x()-nodeWidth<0){
        leftTop2=QPointF(this->width()-nodeWidth,snake2[0].y());
        rightBottom2=QPointF(this->width(),snake2[0].y()+nodeHeight);
    }
    else{
        leftTop2=snake2[0].topLeft()-QPointF(nodeWidth,0);
        rightBottom2=snake2[0].bottomLeft();
    }
     snake2.insert(0,QRectF(leftTop2,rightBottom2));
}

void Widget::addRight(){
    QPointF leftTop;
    QPointF rightBottom;

    if(snake[0].x()+2*nodeWidth>this->width()){
        leftTop=QPointF(0,snake[0].y());
        rightBottom=QPointF(nodeWidth,snake[0].y()+nodeHeight);
    }
    else{
        leftTop=snake[0].topRight();
        rightBottom=snake[0].bottomRight()+QPointF(nodeWidth,0);
    }
     snake.insert(0,QRectF(leftTop,rightBottom));
}

void Widget::addRight2(){
    QPointF leftTop2;
    QPointF rightBottom2;

    if(snake2[0].x()+2*nodeWidth>this->width()){
        leftTop2=QPointF(0,snake2[0].y());
        rightBottom2=QPointF(nodeWidth,snake2[0].y()+nodeHeight);
    }
    else{
        leftTop2=snake2[0].topRight();
        rightBottom2=snake2[0].bottomRight()+QPointF(nodeWidth,0);
    }
     snake2.insert(0,QRectF(leftTop2,rightBottom2));
}

void Widget::deleteLast(){
    snake.removeLast();
}

void Widget::deleteLast2(){
    snake2.removeLast();
}

void Widget::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    QPen pen;
    QBrush brush;

    //背景图片
    QPixmap pix;
    pix.load("C:/Users/21654/Desktop/qt/test3/background.jpg");
    painter.drawPixmap(0,0,2400,1600,pix);

    //画蛇
    pen.setColor(Qt::black);
    brush.setColor(Qt::darkGreen);
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    painter.setPen(pen);

    for(int i=0;i<snake.length();i++){
        painter.drawRect(snake[i]);
    }

    //画第二条蛇
    pen.setColor(Qt::black);
    brush.setColor(Qt::darkBlue);
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    painter.setPen(pen);

    for(int i=0;i<snake2.length();i++){
        painter.drawRect(snake2[i]);
    }

    //画奖品
        pen.setColor(Qt::black);
        brush.setColor(Qt::darkRed);
        brush.setStyle(Qt::SolidPattern);
        painter.setBrush(brush);
        painter.setPen(pen);
        painter.drawEllipse(rewardNode);

         QFont font1("方正舒体",30,QFont::ExtraLight,false);
         QFont font2("宋体",18,QFont::ExtraLight,false);


        if(chectContact()){
           painter.setFont(font1);
           painter.drawText((this->width()-600)/2,(this->height()-50)/2,
                            QString("Game Over!"));
           timer->stop();
        }

        //得分
        pen.setColor(Qt::white);
            painter.setPen(pen);
            painter.setFont(font2);
            painter.drawText(2000,80,QString("绿方当前得分：")+QString("%1").arg(snake.length()-3));
            painter.drawText(2000,160,QString("蓝方当前得分：")+QString("%1").arg(snake2.length()-3));

         //画墙
            pen.setColor(Qt::white);
            brush.setColor(Qt::darkYellow);
            brush.setStyle(Qt::SolidPattern);
            painter.setBrush(brush);
            painter.setPen(pen);
            for(int j=0;j<borderNode.length();j++)
                painter.drawRect(borderNode[j]);


    QWidget::paintEvent(event);
}

void Widget::addNewReward(){
    rewardNode=QRectF(
                (qrand()%((this->width()-450)/nodeWidth))*nodeWidth,
                (qrand()%(this->height()/nodeHeight))*nodeHeight,
                nodeWidth+20,nodeHeight+20);
}

bool Widget::chectContact(){
    for(int j=0;j<snake.length()-1;j++){
    for(int i=j+1;i<snake.length();i++){
        if(snake[j]==snake[i]){
            return true;
        }
    }}
    for(int j=0;j<snake2.length()-1;j++){
    for(int i=j+1;i<snake2.length();i++){
        if(snake2[j]==snake2[i]){
            return true;
        }
    }}
    for(int j=0;j<snake.length();j++){
    for(int i=0;i<snake2.length();i++){
        if(snake[j]==snake2[i]){
            return true;
        }
    }}
    for(int i=0;i<snake.length();i++){
        for(int j=0;j<borderNode.length();j++){
            if(snake[i]==borderNode[j]){
                return true;
            }
        }}
    for(int i=0;i<snake2.length();i++){
        for(int j=0;j<borderNode.length();j++){
            if(snake2[i]==borderNode[j]){
                return true;
            }
        }}
        return false;
}

bool Widget::checkcover(QPointF leftTop){
    for(int i=0;i<snake.length();i++){
            if(snake[i].topLeft()==leftTop)
                return true;
        }
    for(int i=0;i<snake2.length();i++){
            if(snake2[i].topLeft()==leftTop)
                return true;
        }
    for(int i=0;i<borderNode.length();i++){
            if(borderNode[i].topLeft()==leftTop)
                return true;
        }
    return false;
}

void Widget::readdborder(QPointF& leftTop){
    if(checkcover(leftTop)){
        leftTop=QPointF(
                    (qrand()%(this->width()/nodeWidth))*nodeWidth,
                    (qrand()%(this->height()/nodeHeight))*nodeHeight);
        readdborder(leftTop);
    }
}

void Widget::addborder(){
    for(int i=0;i<25;i++){
    int index=borderNode.length();
    QPointF leftTop=QPointF(
                (qrand()%(this->width()/nodeWidth))*nodeWidth,
                (qrand()%(this->height()/nodeHeight))*nodeHeight);
    readdborder(leftTop);
    QPointF rightBottom=leftTop+QPointF(nodeWidth,nodeHeight);
    borderNode.insert(index-1,QRectF(leftTop,rightBottom));
    }
}

void Widget::on_pushButton_2_clicked()//文件夹
{
    //点击文件夹后打开音乐所存储的路径
    auto path=QFileDialog::getExistingDirectory(this,"请选择音乐所在的目录","C:/Users");
    qInfo()<<path;
    //根据该路径，获取里面所有的wav、mp3、flac文件
    QDir dir(path);
    //获取所有满足
    auto musiclist = dir.entryList(QStringList()<<"*.mp3"<<"*.wav"<<"*.flac");
    qInfo()<< musiclist;
    //把音乐名字放到listwidget中展示
    ui->listWidget->addItems(musiclist);
    //默认选中第一个音乐
    ui->listWidget->setCurrentRow(0);

    //把音乐完整路径保存
    for(auto file:musiclist)
        playList.append(QUrl::fromLocalFile(path+"/"+file));

}

void Widget::on_pushButton_3_clicked()//上一首
{if(curPlayIndex == 0)
        curPlayIndex=playList.size()-1;
    else
        curPlayIndex--;

    ui->listWidget->setCurrentRow(curPlayIndex);
    mediaPlayer->setMedia(playList[curPlayIndex-1]);
    mediaPlayer->play();
}

void Widget::on_pushButton_4_clicked()//开始播放
{
    switch(mediaPlayer->state()){
    case QMediaPlayer::StoppedState:{
        //播放当前选中的音乐
        //1.获取选中的行号
        int curPlayIndex = ui->listWidget->currentRow();
        //2.播放对应下标注的音乐
        mediaPlayer->setMedia(playList[curPlayIndex]);
        mediaPlayer->play();
        break;
    }
     //如果现在正在播放，需要暂停音乐
    case QMediaPlayer::PlayingState:
        mediaPlayer->pause();
        break;
    //如果现在暂停，需要继续播放音乐
    case QMediaPlayer::PausedState:
        mediaPlayer->play();
        break;
    default:
        break;
    }
}

void Widget::on_pushButton_5_clicked()//下一首
{
    if(curPlayIndex >=playList.size())
            curPlayIndex=0;
        else
            curPlayIndex++;

        ui->listWidget->setCurrentRow(curPlayIndex);
        mediaPlayer->setMedia(playList[curPlayIndex-1]);
        mediaPlayer->play();
}

void Widget::on_listWidget_doubleClicked(const QModelIndex &index)
{
    curPlayIndex=index.row();
    mediaPlayer->setMedia(playList[curPlayIndex-1]);
    mediaPlayer->play();
}
