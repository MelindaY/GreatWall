#ifndef DBSERVER_H
#define DBSERVER_H

#include  <QtSql>
class Config;

class DBServer
{
    private:
        Config* config;//�����ļ�
    public:
        DBServer(Config* config);
        ~DBServer(void);
        bool createTable();//�������ݱ�
        bool connectToMySQL();//��������
        //int insertMySQL(int nodeID,double voltage,double light,double temperature,double humidity,QString timeValue);//�������ݿ�
        int insertMySQL_doubledata(QString timeValue,double dataValue,int nodeID,int dataFlag);//�������ݿ�
        int insertMySQL_hop(QString timeValue,int hop[5],int nodeID,int PacketNum);//������·�����ѷ����ݰ������������ݿ�
        int deleteItem_hop(int nodeID);//����hopnewest���ݿ�
        int insertMySQL_hopnewest(QString timeValue,int hopnum,int hop[5],int nodeID,int PacketNum);//������·�����ѷ����ݰ������������ݿ�
        int deleteItem_data(int nodeID,int dataFlag);//����datanewest���ݿ�
        int insertMySQL_datanewest(QString timeValue,double dataValue,int nodeID,int dataFlag,int stateFlag);//������·�����ѷ����ݰ������������ݿ�
        int insertMySQL_sioltemphumiTest(QString timeValue,int addrCode1,int addrCode2,int symbol_30cm,int temp1_30cm,int temp2_30cm,int humi1_30cm,int humi2_30cm,int symbol_15cm,int temp1_15cm,int temp2_15cm,int humi1_15cm,int humi2_15cm,int symbol_5cm,int temp1_5cm,int temp2_5cm,int humi1_5cm,int humi2_5cm,int voltagetest,int nodeID);//��������ʪ��ԭʼʮ�������ݲ������ݱ�
        int insertMySQL_gatewaystate(QString timeValue,int flag);
        int insertMySQL_shakestate(QString timeValue,int nodeid);

        int insertMySQL_saltTest(QString ctime,int addrCode,int symbol_30cm,int temp_30cm,int humi_30cm,int vertSalt,int surfSalt,int voltagetest,int nodeid);
        int insertMySQL_shakeTest(QString ctime,int shakeOne,int shakeTwo,int shakeThree,int voltagetest,int nodeid);
        int deletetest_Control(int nodeID);
        int insertMySQL_testControl(QString ctime,int nodeID, int count, int value );
        int insertMySQL_testData(QString ctime, int count, int value );
};
#endif // DBSERVER_H
