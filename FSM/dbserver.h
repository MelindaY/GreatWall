#ifndef DBSERVER_H
#define DBSERVER_H

#include  <QtSql>
class Config;

class DBServer
{
    private:
        Config* config;//配置文件
        QDateTime standardtime;
        QString storageTime_v1;
    public:
        DBServer(Config* config);
        ~DBServer(void);
        bool createTable();//创建数据表
        bool connectToMySQL();//创建连接
        //int insertMySQL(int nodeID,double voltage,double light,double temperature,double humidity,QString timeValue);//插入数据库
        int insertMySQL_originPac(QString ctime,QString packet,int flag,QString ctimeRecv);

};
#endif // DBSERVER_H
