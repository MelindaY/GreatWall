#ifndef DBSERVER_H
#define DBSERVER_H

#include  <QtSql>
class Config;

class DBServer
{
    private:
        Config* config;//配置文件
    public:
        DBServer(Config* config);
        ~DBServer(void);
        bool createTable();//创建数据表
        bool connectToMySQL();//创建连接
        //int insertMySQL(int nodeID,double voltage,double light,double temperature,double humidity,QString timeValue);//插入数据库
        int insertMySQL_doubledata(QString timeValue,double dataValue,int nodeID,int dataFlag);//插入数据库
        int insertMySQL_hop(QString timeValue,int hop[5],int nodeID,int PacketNum);//将跳步路径和已发数据包总数插入数据库
        int deleteItem_hop(int nodeID);//更新hopnewest数据库
        int insertMySQL_hopnewest(QString timeValue,int hopnum,int hop[5],int nodeID,int PacketNum);//将跳步路径和已发数据包总数插入数据库
        int deleteItem_data(int nodeID,int dataFlag);//更新datanewest数据库
        int insertMySQL_datanewest(QString timeValue,double dataValue,int nodeID,int dataFlag,int stateFlag);//将跳步路径和已发数据包总数插入数据库
        int insertMySQL_sioltemphumiTest(QString timeValue,int addrCode1,int addrCode2,int symbol_30cm,int temp1_30cm,int temp2_30cm,int humi1_30cm,int humi2_30cm,int symbol_15cm,int temp1_15cm,int temp2_15cm,int humi1_15cm,int humi2_15cm,int symbol_5cm,int temp1_5cm,int temp2_5cm,int humi1_5cm,int humi2_5cm,int voltagetest,int nodeID);//将土壤温湿度原始十进制数据插入数据表
        int insertMySQL_gatewaystate(QString timeValue,int flag);
        int insertMySQL_shakestate(QString timeValue,int nodeid);

        int insertMySQL_saltTest(QString ctime,int addrCode,int symbol_30cm,int temp_30cm,int humi_30cm,int vertSalt,int surfSalt,int voltagetest,int nodeid);
        int insertMySQL_shakeTest(QString ctime,int shakeOne,int shakeTwo,int shakeThree,int voltagetest,int nodeid);
        int deletetest_Control(int nodeID);
        int insertMySQL_testControl(QString ctime,int nodeID, int count, int value );
        int insertMySQL_testData(QString ctime, int count, int value );
};
#endif // DBSERVER_H
