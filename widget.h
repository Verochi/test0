#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QKeyEvent>
#include <QTimer>
#include <QUrl>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE
class QMediaPlayer;

enum Direct{
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP,
    DIR_DOWN
};//方向的枚举变量

enum Direct2{
    DIR_LEFT2,
    DIR_RIGHT2,
    DIR_UP2,
    DIR_DOWN2
};//第二条蛇的方向枚举变量

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void addUp();
    void addDown();
    void addleft();
    void addRight();

    void addNewReward();
    void addborder();
    void readdborder(QPointF&);

    bool chectContact();
    bool checkcover(QPointF);

    void addUp2();
    void addDown2();
    void addleft2();
    void addRight2();
    //第二条蛇的不同方向移动函数

    //按键处理
    void keyPressEvent(QKeyEvent *event);
    void deleteLast();
    void deleteLast2();
    void keyReleaseEvent(QKeyEvent *event);

    //渲染绘图
    void paintEvent(QPaintEvent *event);

private:
   Ui::Widget *ui;
   QList<QUrl>playList;  //播放列表
   QMediaPlayer *mediaPlayer;
   int curPlayIndex =1;
   int moveFlag = DIR_UP;
   bool gamestart=false;
   int moveFlag2 = DIR_UP2;//第二条蛇的方向标记

   //定时器
   QTimer *timer;
   int time=150;

   //蛇的表示
   QList<QRectF> snake;//表示一块区域
   QList<QRectF> snake2;//表示第二条蛇的区域

   //小方块宽高
   int nodeWidth=20;
   int nodeHeight=20;

   //奖品
   QRectF rewardNode;

   //墙
   QList<QRectF> borderNode;

   //长按是否重复
   bool dPressed=false;

protected slots:
   void timeout();

private slots:
   void on_pushButton_2_clicked();
   void on_pushButton_3_clicked();
   void on_pushButton_4_clicked();
   void on_pushButton_5_clicked();
   void on_listWidget_doubleClicked(const QModelIndex &index);
};
#endif // WIDGET_H
