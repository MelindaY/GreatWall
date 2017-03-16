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
    void setConfig(Config* config);//��ʼ�������е�config
    void createDataBase(Config* config);//ʹ��config�������ݿ�
signals:
    void updateLinkNum(int linkNum);
    //��MainWindow���е�recvupdateLinkNum�����Ӧ���ź�
    void newClientComing();
    //��MainWindow���е�recvNewClientComing�����Ӧ���ź�
    void server_dataInfo(int id,QString gateway,int totalFlow,
                         int storageNum,int exceptNum,
                         QString storageTime,int nodeID,
                         QString arriveTime);
    //��MainWindow���е�updateDataInfo�����Ӧ���ź�
    void RuningState(QString Info);
    void stateInfo(QString state);
    //��MainWindow���е�recvWarning�ۺ�recvStateInfo�����Ӧ���ź�
    void updateTimeStamp(QString timestamp,QString storageTime);
public slots:
    void recvLinkRip();//���ͻ������ӶϿ�ʱ��Ҫ�޸�������
    void recvDataInfo(int id,QString gateway,int totalFlow,
                      int storageNum,int exceptNum,
                      QString storageTime,int nodeID,
                      QString arriveTime);
    //���ս����̷߳������źţ�������server_dataInfo�źţ�ʵʱ���½�������
    void recvTimeStamp(QString timestamp,QString storageTime);
protected:
    void incomingConnection(int handle);//�����������ӵ���ʱ
private:
    QList<MyThread*> clientSocketList;//�����б�
    int linkNum;//������
    DBServer* dataBase;//���ݿ�,���ݿⱸ��
    DBServer* dataBaseBackup;
    Config* config;//�����ļ�������������ļ���Ҫ�������������ݿ�



};

#endif // MYSERVER_H
