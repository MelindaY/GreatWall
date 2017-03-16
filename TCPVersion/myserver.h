#ifndef MYSERVER_H
#define MYSERVER_H

#include <QTcpServer>
//#include <QList>
#include <QTcpSocket>
#include "mythread.h"
#include "config.h"

class DBServer;
class Config;
class MyServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MyServer(QObject *parent = 0);
    void setConfig(Config* config);//初始化此类中的config
    void createDataBase(Config* config);//使用config连接数据库
signals:
    void updateLinkNum(int linkNum);
    //与MainWindow类中的recvupdateLinkNum槽相对应的信号
    void newClientComing();
    //与MainWindow类中的recvNewClientComing槽相对应的信号
    void server_dataInfo(int id,QString gateway,int totalFlow,
                         int storageNum,int exceptNum,
                         QString storageTime,int nodeID,
                         QString arriveTime);
    //与MainWindow类中的updateDataInfo槽相对应的信号
    void RuningState(QString Info);
    void stateInfo(QString state);
    //与MainWindow类中的recvWarning槽和recvStateInfo槽相对应的信号
    void updateTimeStamp(QString timestamp,QString storageTime);
public slots:
    void recvLinkRip();//当客户端连接断开时，要修改连接数
    void recvDataInfo(int id,QString gateway,int totalFlow,
                      int storageNum,int exceptNum,
                      QString storageTime,int nodeID,
                      QString arriveTime);
    //接收接收线程发来的信号，并发出server_dataInfo信号，实时更新界面数据
    void recvTimeStamp(QString timestamp,QString storageTime);
protected:
    void incomingConnection(int handle);//当新网关连接到来时
private:
    QList<MyThread*> clientSocketList;//网关列表
    int linkNum;//连接数
    DBServer* dataBase;//数据库,数据库备份
    DBServer* dataBaseBackup;
    Config* config;//配置文件，这里的配置文件主要是用来连接数据库



};

#endif // MYSERVER_H
