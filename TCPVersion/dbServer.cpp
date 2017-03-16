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
    db.setConnectOptions("MYSQL_OPT_RECONNECT=1");
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

bool DBServer::connectToMySQLBackup()//连接备用数据库
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL3");
    db.setDatabaseName(config->getMysql_dbname());
    db.setConnectOptions("MYSQL_OPT_RECONNECT=1");
    db.setHostName(config->getIPBackup());
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

int DBServer::insertMySQL_doubledata(QString timeValue,double dataValue,int nodeID,int dataFlag,QString storageTime_v1)//插入数据库
{
    storageTime_v1.clear();
    standardtime=QDateTime::currentDateTime();
    storageTime_v1=standardtime.toString("yyyyMMddhhmmsszzz"); //设置显示格式
    storageTime_v1+="000";

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
    mySQL+=",";
    mySQL+=storageTime_v1;
    mySQL+=")";

    query.exec(mySQL);

    QSqlError error;
    QString errorLog;
    errorLog.clear();
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
            fileOut<<exceptTime+"   "+"datanumError"+"   "+errorLog<<"\r\n";
            file.close();
        }
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

    QSqlError error;
    QString errorLog;
    errorLog.clear();
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
            fileOut<<exceptTime+" "+"soiltemphumiError"+"   "+errorLog<<"\r\n";
            file.close();
        }
    }
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

    QSqlError error;
    QString errorLog;
    errorLog.clear();
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
            fileOut<<exceptTime+"   "+" "+"salttest"+errorLog<<"\r\n";
            file.close();
        }
    }

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
int DBServer::insertMySQL_testControl(QString ctime,double count, int nodeid, int value,QString storageTime_v1){

    storageTime_v1.clear();
    standardtime=QDateTime::currentDateTime();
    storageTime_v1=standardtime.toString("yyyyMMddhhmmsszzz"); //设置显示格式
    storageTime_v1+="000";
    QSqlQuery query;
    QString mySQL="INSERT INTO control_pac VALUES(";
    mySQL+=ctime;
    mySQL+=",";
    mySQL+=QString::number(count);
    mySQL+=",";
    mySQL+=QString::number(nodeid);
    mySQL+=",";
    mySQL+=QString::number(value);
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

    return 0;
}

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
    if(query.exec(mySQL)==false)
    {
         QMessageBox::critical(0,"Error",query.lastError().text());
    }
}
int DBServer::insertMySQL_rainTest(QString ctime,int rainnode_addr,int tem_outside_int,int tem_outside_dec,
                         int tem_inside_int,int tem_inside_dec,int phInt,int phDec,int rainInt,
                         int rainDec,int windspeedInt,int windspeedDec,int winddirectionInt,
                         int winddirectionDec,int powerInt,int powerDec,int rainFall, int fVoltage,
                         int nodeID)

{
    QSqlQuery query;
    QString mySQL="INSERT INTO raintest VALUES(";
    mySQL+=ctime;
    mySQL+=",";
    mySQL+=QString::number(rainnode_addr);
    mySQL+=",";
    mySQL+=QString::number(tem_outside_int);
    mySQL+=",";
    mySQL+=QString::number(tem_outside_dec);
    mySQL+=",";
    mySQL+=QString::number(tem_inside_int);
    mySQL+=",";
    mySQL+=QString::number(tem_inside_dec);
    mySQL+=",";
    mySQL+=QString::number(phInt);
    mySQL+=",";
    mySQL+=QString::number(phDec);
    mySQL+=",";
    mySQL+=QString::number(rainInt);
    mySQL+=",";
    mySQL+=QString::number(rainDec);
    mySQL+=",";
    mySQL+=QString::number(windspeedInt);
    mySQL+=",";
    mySQL+=QString::number(windspeedDec);
    mySQL+=",";
    mySQL+=QString::number(winddirectionInt);
    mySQL+=",";
    mySQL+=QString::number(winddirectionDec);
    mySQL+=",";
    mySQL+=QString::number(powerInt);
    mySQL+=",";
    mySQL+=QString::number(powerDec);
    mySQL+=",";
    mySQL+=QString::number(rainFall);
    mySQL+=",";
    mySQL+=QString::number(fVoltage);
    mySQL+=",";
    mySQL+=QString::number(nodeID);
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
            fileOut<<exceptTime+" "+"control_pac"+"   "+errorLog<<"\r\n";
            file.close();
        }
    }

}
int DBServer::inserMySQL_soildata(QString ctime,int addressCode1,int addressCode2, double tem_30cm,double hum_30cm,double tem_15cm,double hum_15cm,double tem_5cm,double hum_5cm,double fVoltage,int nodeID)
{
    QSqlQuery query;
    //组合成sql的插入命令
    QString mySQL="INSERT INTO soildata VALUES(";
    mySQL+=ctime;
    mySQL+=",";
    mySQL+=QString::number(addressCode1);
    mySQL+=",";
    mySQL+=QString::number(addressCode2);
    mySQL+=",";
    mySQL+=QString::number(tem_30cm);
    mySQL+=",";
    mySQL+=QString::number(hum_30cm);
    mySQL+=",";
    mySQL+=QString::number(tem_15cm);
    mySQL+=",";
    mySQL+=QString::number(hum_15cm);
    mySQL+=",";
    mySQL+=QString::number(tem_5cm);
    mySQL+=",";
    mySQL+=QString::number(hum_5cm);
    mySQL+=",";
    mySQL+=QString::number(fVoltage);
    mySQL+=",";
    mySQL+=QString::number(nodeID);
    mySQL+=")";

    query.exec(mySQL);
    QSqlError error;
    QString errorLog;
    errorLog.clear();
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
            fileOut<<exceptTime+" "+"soildata"+"   "+errorLog<<"\r\n";
            file.close();
        }
    }

}

int DBServer::selectMySQL_paras(double parameters[150][14],int &count)
{
    QSqlQuery query;
    //double addr1,addr2,A30,K30,D30,M30,A15,K15,D15,M15,A5,K5,D5,M5;
    //组合成sql的查询命令
   // QString mySQL="select * from soittemphumiparameters where ADDR2=";
   // mySQL+=QString::number(addrCode2);

    QString mySQL="select * from soittemphumiparameters";

    query.exec(mySQL);

    count=0;
    while(query.next())
    {
        parameters[count][0]=query.value(0).toDouble();
        parameters[count][1]=query.value(1).toDouble();
        parameters[count][2]=query.value(2).toDouble();
        parameters[count][3]=query.value(3).toDouble();
        parameters[count][4]=query.value(4).toDouble();
        parameters[count][5]=query.value(5).toDouble();
        parameters[count][6]=query.value(6).toDouble();
        parameters[count][7]=query.value(7).toDouble();
        parameters[count][8]=query.value(8).toDouble();
        parameters[count][9]=query.value(9).toDouble();

        parameters[count][10]=query.value(10).toDouble();
        parameters[count][11]=query.value(11).toDouble();
        parameters[count][12]=query.value(12).toDouble();
        parameters[count][13]=query.value(13).toDouble();
        count++;
    }
    QSqlError error;
    QString errorLog;
    errorLog.clear();
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
            fileOut<<exceptTime+"   "+errorLog<<"\r\n";
            file.close();
        }
    }

    return 0;

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
int DBServer::selectMySQL_saltParas(double parameters[109][2],int &count)
{
    QSqlQuery query;
    //double addr1,addr2,A30,K30,D30,M30,A15,K15,D15,M15,A5,K5,D5,M5;
    //组合成sql的查询命令
   // QString mySQL="select * from soittemphumiparameters where ADDR2=";
   // mySQL+=QString::number(addrCode2);

    QString mySQL="select * from saltparameters";

    query.exec(mySQL);

    count=0;
    while(query.next())
    {
        parameters[count][0]=query.value(0).toDouble();
        parameters[count][1]=query.value(1).toDouble();
        count++;
    }


    QSqlError error;
    QString errorLog;
    errorLog.clear();
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
            fileOut<<"para"<<exceptTime+"   "+errorLog<<"\r\n";
            file.close();
        }
    }

    return 0;

}
 //find the latested time figure of rain
int DBServer::selectMySQL_rainCtime(QDateTime &ctime, int nodeid)
{

    QSqlQuery query;
    QString lastestTime;

    QString mySQL="SELECT `datanum`.`ctime` FROM datanum WHERE datatype=13 and nodeid= ";
    mySQL+=QString::number(nodeid);
    mySQL+=" ORDER BY ctime DESC LIMIT 0,1";

    query.exec(mySQL);
//    if(!query.next())
//        firstFlag=1;
//    else
//        firstFlag=0;

    while(query.next())
    {
        lastestTime=query.value(0).toString();
        int i=lastestTime.indexOf("000");
        lastestTime.remove(i,3);
        ctime=QDateTime::fromString(lastestTime,"yyyyMMddhhmmsszzz");
    }


    QSqlError error;
    QString errorLog;
    errorLog.clear();
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
            fileOut<<exceptTime<<" "+errorLog<<"\r\n";
            file.close();
        }
    }
    return 0;

}
int DBServer::insertMYSQL_rainAnalysis(QString ctime,int rainFlag,double rainFall,double raininess,int nodeid)
{


    QSqlQuery query;
    //组合成sql的插入命令
    QString mySQL="INSERT INTO rainAnalysis VALUES(";
    mySQL+=ctime;
    mySQL+=",";
    mySQL+=QString::number(rainFlag);
    mySQL+=",";
    mySQL+=QString::number(rainFall);
    mySQL+=",";
    mySQL+=QString::number(raininess);
    mySQL+=",";
    mySQL+=QString::number(nodeid);
    mySQL+=")";

    query.exec(mySQL);

    QSqlError error;
    QString errorLog;
    errorLog.clear();
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
            fileOut<<exceptTime+"   "+"datanumError"+"   "+errorLog<<"\r\n";
            file.close();
        }
    }
    return 0;
}
int DBServer::selectMySQL_voltagePrevious(QString dayPrevious,double &voltage,int nodeid)
{
    QSqlQuery query;
    //组合成sql的插入命令
    QString mySQL="SELECT data FROM datanum where datatype=16 and substring(ctime,5,6)= ";
    mySQL+=dayPrevious;
    mySQL+=" AND nodeid= ";
    mySQL+=QString::number(nodeid);
    mySQL+=" ORDER BY ctime asc LIMIT 0,1";

    query.exec(mySQL);


    if(query.next())
    {
        voltage=query.value(0).toDouble();
    }
    else
        voltage=10000;

    QSqlError error;
    QString errorLog;
    errorLog.clear();
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
            fileOut<<exceptTime+"   "+"raintestError"+"   "+errorLog<<"\r\n";
            file.close();
        }
    }
    return 0;
}
