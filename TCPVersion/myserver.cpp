#include "myserver.h"
#include "mythread.h"
#include "dbServer.h"

MyServer::MyServer(QObject *parent) :
    QTcpServer(parent)
{
    clientSocketList.clear();
    this->linkNum=0;//��������ʼ��Ϊ0
}

void MyServer::setConfig(Config* config)
{
    this->config=config;
}

void MyServer::createDataBase(Config* config)
{
    this->dataBase=new DBServer(config);
    this->dataBaseBackup=new DBServer(config);
    //�������ݿ�
    if(dataBase->connectToMySQL()==false||dataBaseBackup->connectToMySQLBackup()==false)

    {
        emit RuningState("error");
    }
    else
    {
        emit stateInfo("Connect to dataBase Success!");
        emit stateInfo("Receive port:"+config->getPort());
        emit stateInfo("IP:"+config->getIP());
        emit stateInfo("Mysql_user:"+config->getMysql_user());
        emit stateInfo("getMysql_dbname:"+config->getMysql_user());
        emit RuningState("right");
    }
    dataBase->createTable();
}

void MyServer::incomingConnection(int handle)
//���ͻ������ӵ���ʱ���˺����Զ����ã�ֱ�Ӵ����߳�
{
    MyThread* newthread=new MyThread;
    newthread->setSocketDescriptor(handle);
    newthread->setMyThreadID(clientSocketList.count());
    newthread->setgateway();
    newthread->setDBServer(this->dataBase);//�������ݿ�ָ��
    newthread->setDBSeverBackup(this->dataBaseBackup);
    newthread->setHandle(handle);
    connect(newthread,SIGNAL(linkRup()),this,SLOT(recvLinkRip()));//,Qt::QueuedConnection);
    connect(newthread,SIGNAL(dataInfo(int,QString,int,int,int,QString,int,QString)),this,SLOT(recvDataInfo(int,QString,int,int,int,QString,int,QString)),Qt::QueuedConnection);
    connect(newthread,SIGNAL(updatetimeStamp(QString,QString)),this,SLOT(recvTimeStamp(QString,QString)));
    newthread->setTimeStamp();
    clientSocketList.append(newthread);
    this->linkNum++;
    emit newClientComing();
    emit updateLinkNum(this->linkNum);
}

void MyServer::recvLinkRip()//���ͻ������ӶϿ�ʱ��Ҫ�޸�������
{
    this->linkNum--;
    emit updateLinkNum(this->linkNum);
}

void MyServer::recvDataInfo(int id,QString gateway,int totalFlow,int storageNum,int exceptNum,QString storageTime,int nodeID,QString arriveTime)//�������ݽ��ղ���
{
    emit server_dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime,nodeID,arriveTime);
}
void MyServer::recvTimeStamp(QString timestamp, QString storageTime)
{
    emit updateTimeStamp(timestamp, storageTime);
}
