#include "dbServer.h"
#include "config.h"
#include<QMessageBox>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlError>

DBServer::DBServer(Config* config)//构造函数
{
    //执行SQL语句
    //创建一个执行数据库SQL语句对象
    //如果存在nodedata数据库表格，先删除
    //query.exec("drop table nodedata");
    //在创建数据库表格nodedata
    this->config=config;
    //config->loadConfig("C:\\Qt\\qtcreator-2.4.1\\MyServer\\config.ini");
    config->loadConfig("config.ini");
}
DBServer::~DBServer()
{

}

bool DBServer::createTable()//创建数据表
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

bool DBServer::connectToMySQL()//连接数据库
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
    if(error.isValid())//发生错误时isValid()返回true
    {

       switch (error.type()) {
       case QSqlError::NoError:
           break;
       case QSqlError::ConnectionError://连接错语
           errorLog=error.text();
           break;
       case QSqlError::StatementError://语句错语
           errorLog=error.text();
           break;
       case QSqlError::TransactionError://事务错误
           errorLog=error.text();
           break;
       default://未知错误
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
            QDateTime currenttime=QDateTime::currentDateTime();//获取系统现在的时间
            QString exceptTime=currenttime.toString("yyyy-MM-dd hh:mm:ss"); //设置显示格式
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
    storageTime_v1=standardtime.toString("yyyyMMddhhmmsszzz"); //设置显示格式
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
    if(error.isValid())//发生错误时isValid()返回true
    {

       switch (error.type()) {
       case QSqlError::NoError:
           break;
       case QSqlError::ConnectionError://连接错语
           errorLog=error.text();
           break;
       case QSqlError::StatementError://语句错语
           errorLog=error.text();
           break;
       case QSqlError::TransactionError://事务错误
           errorLog=error.text();
           break;
       default://未知错误
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
            QDateTime currenttime=QDateTime::currentDateTime();//获取系统现在的时间
            QString exceptTime=currenttime.toString("yyyy-MM-dd hh:mm:ss"); //设置显示格式
            fileOut<<exceptTime+" "+"orgindata"+"   "+errorLog<<"\r\n";
            file.close();
        }
    }
}
