#ifndef DBSERVER_H
#define DBSERVER_H

#include  <QtSql>
class Config;

class DBServer
{
    private:
        Config* config;//�����ļ�
        QDateTime standardtime;
        QString storageTime_v1;
    public:
        DBServer(Config* config);
        ~DBServer(void);
        bool createTable();//�������ݱ�
        bool connectToMySQL();//��������
        //int insertMySQL(int nodeID,double voltage,double light,double temperature,double humidity,QString timeValue);//�������ݿ�
        int insertMySQL_originPac(QString ctime,QString packet,int flag,QString ctimeRecv);

};
#endif // DBSERVER_H
