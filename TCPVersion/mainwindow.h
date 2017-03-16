#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class MyServer;
class Config;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);//构造函数
    ~MainWindow();//析构函数
public slots:
    void recvupdateLinkNum(int linkNum);
    //更新界面的LinkNum值
    void updateDataInfo(int id,QString gateway,int totalFlow,
                        int storageNum,int exceptNum,
                        QString storageTime,int nodeID,
                        QString arriveTime);
    //更新界面显示数值
    void recvNewClientComing();
    //当新连接到来时，增加Realtime Status里面的表格行数
    void recvRuningState(QString info);//在界面上显示警告信息
    void recvStateInfo(QString state);//在界面上显示服务状态信息
    void qtimeSlot();//时间更新
    void recvTimeStamp(QString stampTime,QString currentTime);

private slots:
    void on_ExitBotton_2_clicked();//界面上退出按钮的实现

private:
    Ui::MainWindow *ui;//界面对象
    MyServer *server; //主线程对象,用于等待网关的到来
    Config *config;//配置文件对象，在这里使用配置文件，主要是获取接收端口
    QTimer *timer;//计时器
};

#endif // MAINWINDOW_H
