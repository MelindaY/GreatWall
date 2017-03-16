#include "dbServer.h"
#include "config.h"
#include<QMessageBox>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlError>

DBServer::DBServer(Config* config)//���캯��
{
    //ִ��SQL���
    //����һ��ִ�����ݿ�SQL������
    //�������nodedata���ݿ�����ɾ��
    //query.exec("drop table nodedata");
    //�ڴ������ݿ���nodedata
    this->config=config;
    //config->loadConfig("C:\\Qt\\qtcreator-2.4.1\\MyServer\\config.ini");
    config->loadConfig("config.ini");
}
DBServer::~DBServer()
{

}

bool DBServer::createTable()//�������ݱ�
{
    QSqlQuery query;
    query.exec("use wsn");
    //query.exec("SELECT Count(*) AS Qty FROM wsn WHERE (((wsn.Name) Like nodedata));");
    //if(Qty==0)
   //     query.exec("CREATE TABLE nodedata(nodeID INT,voltage DOUBLE,light DOUBLE,temperature DOUBLE,humidity DOUBLE)");
//    {
//        QMessageBox::critical(0,"Error","create wrong!");
//    }
     return true;
}

bool DBServer::connectToMySQL()//�������ݿ�
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL3");
    db.setDatabaseName(config->getMysql_dbname());
    db.setHostName(config->getIP());
    //db.setPort((config->getPort());
    db.setUserName(config->getMysql_user());
    db.setPassword(config->getMysql_psw());
    bool ok = db.open();
    if(!ok)
    {
        QMessageBox::critical(0,"Error",db.lastError().text());
        return false;
    }

    QSqlError error;
    QString errorLog;
    errorLog.clear();
    error=db.lastError();
    if(error.isValid())//��������ʱisValid()����true
    {

       switch (error.type()) {
       case QSqlError::NoError:
           break;
       case QSqlError::ConnectionError://���Ӵ���
           errorLog=error.text();
           break;
       case QSqlError::StatementError://������
           errorLog=error.text();
           break;
       case QSqlError::TransactionError://�������
           errorLog=error.text();
           break;
       default://δ֪����
          errorLog=error.text();
           break;
       }
    }

    if(!errorLog.isEmpty())
    {
        QFile file("DatabaseError.txt");
        if(file.open(QIODevice::WriteOnly|QIODevice::WriteOnly|QIODevice::Append))
        {
            QTextStream fileOut(&file);
            QDateTime currenttime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
            QString exceptTime=currenttime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��ʽ
            fileOut<<exceptTime+"   "+"connectionError"+"   "+errorLog<<"\r\n";
            file.close();
        }
    }

    return true;
}
int DBServer::insertMySQL_originPac(QString ctime,QString packet,int flag,QString storageTime_v1)
{
    storageTime_v1.clear();
    standardtime=QDateTime::currentDateTime();
    storageTime_v1=standardtime.toString("yyyyMMddhhmmsszzz"); //������ʾ��ʽ
    storageTime_v1+="000";
    QSqlQuery query;
    QString mySQL="INSERT INTO orginaldata VALUES(";
    mySQL+=ctime;
    mySQL+=",";
    mySQL+="\'";
    mySQL+=packet;
    mySQL+="\'";
    mySQL+=",";
    mySQL+=QString::number(flag);
    mySQL+=",";
    mySQL+=storageTime_v1;
    mySQL+=")";
    query.exec(mySQL);

    QSqlError error;
    QString errorLog;
    error=query.lastError();
    if(error.isValid())//��������ʱisValid()����true
    {

       switch (error.type()) {
       case QSqlError::NoError:
           break;
       case QSqlError::ConnectionError://���Ӵ���
           errorLog=error.text();
           break;
       case QSqlError::StatementError://������
           errorLog=error.text();
           break;
       case QSqlError::TransactionError://�������
           errorLog=error.text();
           break;
       default://δ֪����
          errorLog=error.text();
           break;
       }
    }
    if(!errorLog.isEmpty())
    {
        QFile file("DatabaseError.txt");
        if(file.open(QIODevice::WriteOnly|QIODevice::WriteOnly|QIODevice::Append))
        {
            QTextStream fileOut(&file);
            QDateTime currenttime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
            QString exceptTime=currenttime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��ʽ
            fileOut<<exceptTime+" "+"orgindata"+"   "+errorLog<<"\r\n";
            file.close();
        }
    }
}
