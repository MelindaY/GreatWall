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
    return true;
}

int DBServer::insertMySQL_doubledata(QString timeValue,double dataValue,int nodeID,int dataFlag)//插入数据库
{
    try
    {
    QSqlQuery query;
    //组合成sql的插入命令
    QString mySQL="INSERT INTO datanum VALUES(";
    mySQL+=timeValue;
    mySQL+=",";
    mySQL+=QString::number(dataValue);
    mySQL+=",";
    mySQL+=QString::number(nodeID);
    mySQL+=",";
    mySQL+=QString::number(dataFlag);


    mySQL+=")";

    query.exec(mySQL);
    }
    catch(QString exception)
    {
        QSqlQuery query_error;
        //组合成sql的插入命令
        QString mySQL_error="INSERT INTO errorreport VALUES(";
        mySQL_error+=timeValue;
        mySQL_error+=",";
        mySQL_error+=QString::number(nodeID);
        mySQL_error+=",";
        mySQL_error+=QString::number(0);
        mySQL_error+=")";

        query_error.exec(mySQL_error);
    }

    return 0;
}

int DBServer::insertMySQL_hop(QString timeValue,int hop[5],int nodeID,int PacketNum)//插入数据库
{
    try
    {
    QSqlQuery query;
    //组合成sql的插入命令
    QString mySQL="INSERT INTO hopnum VALUES(";
    mySQL+=timeValue;
    mySQL+=",";
    for(int i=0;i<5;i++)
    {
        mySQL+=QString::number(hop[i]);
        mySQL+=",";
    }
    mySQL+=QString::number(nodeID);
    mySQL+=",";
    mySQL+=QString::number(PacketNum);
    mySQL+=")";

    query.exec(mySQL);
    }
    catch(QString exception)
    {
        QSqlQuery query_error;
        //组合成sql的插入命令
        QString mySQL_error="INSERT INTO errorreport VALUES(";
        mySQL_error+=timeValue;
        mySQL_error+=",";
        mySQL_error+=QString::number(nodeID);
        mySQL_error+=",";
        mySQL_error+=QString::number(1);
        mySQL_error+=")";

        query_error.exec(mySQL_error);
    }

    return 0;
}

int DBServer::deleteItem_hop(int nodeID)
{
    QSqlQuery query;
    //组合成sql的删除命令
    QString mySQL="delete from hopnewest where nodeid =";
    mySQL+=QString::number(nodeID);

    query.exec(mySQL);

    return 0;
}
int DBServer::insertMySQL_hopnewest(QString timeValue,int hopnum,int hop[5],int nodeID,int PacketNum)//将跳步路径和已发数据包总数插入数据库
{
    try
    {
    QSqlQuery query;
    //组合成sql的插入命令
    QString mySQL="INSERT INTO hopnewest VALUES(";
    mySQL+=timeValue;
    mySQL+=",";
    mySQL+=QString::number(hopnum);
    mySQL+=",";
    for(int i=0;i<5;i++)
    {
        mySQL+=QString::number(hop[i]);
        mySQL+=",";
    }
    mySQL+=QString::number(nodeID);
    mySQL+=",";
    mySQL+=QString::number(PacketNum);
    mySQL+=")";

    query.exec(mySQL);
    }
    catch(QString exception)
    {
        QSqlQuery query_error;
        //组合成sql的插入命令
        QString mySQL_error="INSERT INTO errorreport VALUES(";
        mySQL_error+=timeValue;
        mySQL_error+=",";
        mySQL_error+=QString::number(nodeID);
        mySQL_error+=",";
        mySQL_error+=QString::number(2);
        mySQL_error+=")";

        query_error.exec(mySQL_error);
    }

    return 0;
}
int DBServer::deleteItem_data(int nodeID,int dataFlag)//更新datanewest数据库
{
    QSqlQuery query;
    //组合成sql的删除命令
    QString mySQL="delete from datanewest where nodeid =";
    mySQL+=QString::number(nodeID);
    mySQL+=" and datatype=";
    mySQL+=QString::number(dataFlag);
    query.exec(mySQL);

    return 0;
}

int DBServer::insertMySQL_datanewest(QString timeValue,double dataValue,int nodeID,int dataFlag,int stateFlag)//将跳步路径和已发数据包总数插入数据库
{
    try
    {
    QSqlQuery query;
    //组合成sql的插入命令
    QString mySQL="INSERT INTO datanewest VALUES(";
    mySQL+=timeValue;
    mySQL+=",";
    mySQL+=QString::number(dataValue);
    mySQL+=",";
    mySQL+=QString::number(nodeID);
    mySQL+=",";
    mySQL+=QString::number(dataFlag);

    mySQL+=",";
    mySQL+=QString::number(stateFlag);
    mySQL+=")";

    query.exec(mySQL);
    }
    catch(QString exception)
    {
        QSqlQuery query_error;
        //组合成sql的插入命令
        QString mySQL_error="INSERT INTO errorreport VALUES(";
        mySQL_error+=timeValue;
        mySQL_error+=",";
        mySQL_error+=QString::number(nodeID);
        mySQL_error+=",";
        mySQL_error+=QString::number(3);
        mySQL_error+=")";

        query_error.exec(mySQL_error);
    }

    return 0;
}

int DBServer::insertMySQL_sioltemphumiTest(QString timeValue,int addrCode1,int addrCode2,int symbol_30cm,int temp1_30cm,int temp2_30cm,int humi1_30cm,int humi2_30cm,int symbol_15cm,int temp1_15cm,int temp2_15cm,int humi1_15cm,int humi2_15cm,int symbol_5cm,int temp1_5cm,int temp2_5cm,int humi1_5cm,int humi2_5cm,int voltagetest,int nodeID)
{
    try
    {
    QSqlQuery query;
    //组合成sql的插入命令
    QString mySQL="INSERT INTO soiltemphumi VALUES(";
    mySQL+=timeValue;
    mySQL+=",";
    mySQL+=QString::number(addrCode1);
    mySQL+=",";
    mySQL+=QString::number(addrCode2);
    mySQL+=",";
    mySQL+=QString::number(symbol_30cm);
    mySQL+=",";
    mySQL+=QString::number(temp1_30cm);
    mySQL+=",";
    mySQL+=QString::number(temp2_30cm);
    mySQL+=",";
    mySQL+=QString::number(humi1_30cm);
    mySQL+=",";
    mySQL+=QString::number(humi2_30cm);
    mySQL+=",";
    mySQL+=QString::number(symbol_15cm);
    mySQL+=",";
    mySQL+=QString::number(temp1_15cm);
    mySQL+=",";
    mySQL+=QString::number(temp2_15cm);
    mySQL+=",";
    mySQL+=QString::number(humi1_15cm);
    mySQL+=",";
    mySQL+=QString::number(humi2_15cm);
    mySQL+=",";
    mySQL+=QString::number(symbol_5cm);
    mySQL+=",";
    mySQL+=QString::number(temp1_5cm);
    mySQL+=",";
    mySQL+=QString::number(temp2_5cm);
    mySQL+=",";
    mySQL+=QString::number(humi1_5cm);
    mySQL+=",";
    mySQL+=QString::number(humi2_5cm);
    mySQL+=",";
    mySQL+=QString::number(voltagetest);
    mySQL+=",";
    mySQL+=QString::number(nodeID);
    mySQL+=")";

    query.exec(mySQL);
    }
    catch(QString exception)
    {
        QSqlQuery query_error;
        //组合成sql的插入命令
        QString mySQL_error="INSERT INTO errorreport VALUES(";
        mySQL_error+=timeValue;
        mySQL_error+=",";
        mySQL_error+=QString::number(nodeID);
        mySQL_error+=",";
        mySQL_error+=QString::number(4);
        mySQL_error+=")";

        query_error.exec(mySQL_error);
    }

    return 0;
}

int DBServer::insertMySQL_gatewaystate(QString timeValue,int flag)
{

    QSqlQuery query;
    //组合成sql的插入命令
    QString mySQL="INSERT INTO gatawaystate VALUES(";
    mySQL+=timeValue;
    mySQL+=",";
    mySQL+=QString::number(flag);
    mySQL+=")";

    query.exec(mySQL);

    return 0;
}

int DBServer::insertMySQL_shakestate(QString timeValue,int nodeid)
{
    QSqlQuery query;
    //组合成sql的插入命令
    QString mySQL="INSERT INTO shakestate VALUES(";
    mySQL+=timeValue;
    mySQL+=",";
    mySQL+=QString::number(nodeid);
    mySQL+=")";

    query.exec(mySQL);

    return 0;
}

int DBServer::insertMySQL_saltTest(QString ctime,int addrCode,int symbol_30cm,int temp_30cm,int humi_30cm,int vertSalt,int surfSalt,int voltagetest,int nodeid)
{
    QSqlQuery query;
    //组合成sql的插入命令
    QString mySQL="INSERT INTO salttest VALUES(";
    mySQL+=ctime;
    mySQL+=",";
    mySQL+=QString::number(addrCode);
    mySQL+=",";
    mySQL+=QString::number(symbol_30cm);
    mySQL+=",";
    mySQL+=QString::number(temp_30cm);
    mySQL+=",";
    mySQL+=QString::number(humi_30cm);
    mySQL+=",";
    mySQL+=QString::number(vertSalt);
    mySQL+=",";
    mySQL+=QString::number(surfSalt);
    mySQL+=",";
    mySQL+=QString::number(voltagetest);
    mySQL+=",";
    mySQL+=QString::number(nodeid);
    mySQL+=")";
    query.exec(mySQL);
}

int DBServer::insertMySQL_shakeTest(QString ctime,int shakeOne,int shakeTwo,int shakeThree,int voltagetest,int nodeid)
{
    QSqlQuery query;
    //组合成sql的插入命令
    QString mySQL="INSERT INTO shaketest VALUES(";
    mySQL+=ctime;
    mySQL+=",";
    mySQL+=QString::number(shakeOne);
    mySQL+=",";
    mySQL+=QString::number(shakeTwo);
    mySQL+=",";
    mySQL+=QString::number(shakeThree);
    mySQL+=",";
    mySQL+=QString::number(voltagetest);
    mySQL+=",";
    mySQL+=QString::number(nodeid);
    mySQL+=")";

    query.exec(mySQL);
}
int DBServer::deletetest_Control(int nodeID)
{
    QSqlQuery query;
    //组合成sql的删除命令
    QString mySQL="delete from control_pac where nodeid =";
    mySQL+=QString::number(nodeID);

    query.exec(mySQL);

    return 0;
}
int DBServer::insertMySQL_testControl(QString ctime, int count, int nodeid, int value){
    QSqlQuery query;
    QString mySQL="INSERT INTO control_pac VALUES(";
    mySQL+=ctime;
    mySQL+=",";
    mySQL+=QString::number(count);
    mySQL+=",";
    mySQL+=QString::number(nodeid);
    mySQL+=",";
    mySQL+=QString::number(value);
    mySQL+=")";
    query.exec(mySQL);

    return 0;
}
//int DBServer::deletetest_Data(int nodeID)
//{
//    QSqlQuery query;
//    //组合成sql的删除命令
//    QString mySQL="delete from data_pac where nodeid =";
//    mySQL+=QString::number(nodeID);

//    query.exec(mySQL);

//    return 0;
//}
int DBServer::insertMySQL_testData(QString ctime, int current_nodeid, int value){
    QSqlQuery query;
    QString mySQL="INSERT INTO data_pac VALUES(";
    mySQL+=ctime;
    mySQL+=",";
    mySQL+=QString::number(current_nodeid);
    //mySQL+=count;
    mySQL+=",";
    //mySQL+=value;
    mySQL+=QString::number(value);
    mySQL+=")";
    query.exec(mySQL);

}
