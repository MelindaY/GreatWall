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
        bool connectToMySQLBackup();//������������
        //int insertMySQL(int nodeID,double voltage,double light,double temperature,double humidity,QString timeValue);//�������ݿ�
        int insertMySQL_doubledata(QString timeValue,double dataValue,int nodeID,int dataFlag,QString ctimeRecv);//�������ݿ�
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
        int insertMySQL_testControl(QString ctime, double count,int nodeID, int value,QString ctimeRecv );
        int insertMySQL_testData(QString ctime, int count, int value );

        int insertMySQL_rainTest(QString ctime,int rainnode_addr,int tem_outside_int,int tem_outside_dec,
                                  int tem_inside_int,int tem_inside_dec,int phInt,int phDec,int rainInt,int rainDec,int windspeedInt,
                                  int windspeedDec,int winddirectionInt,int winddirectionDec,int powerInt,int powerDec,int rainFall, int fVoltage,int nodeID);
        int selectMySQL_paras(double parameters[150][14],int& count);
        int inserMySQL_soildata(QString ctime,int addressCode1,int addressCode2, double tem_30cm,double hum_30cm,double tem_15cm,double hum_15cm,double tem_5cm,double hum_5cm,double fVoltage,int nodeID);
        int insertMySQL_originPac(QString ctime,QString packet,int flag,QString ctimeRecv);
        int selectMySQL_saltParas(double parameters[109][2],int &count);
        int selectMySQL_rainCtime(QDateTime &ctime,int nodeid);
        int insertMYSQL_rainAnalysis(QString ctime,int rainFlag,double rainFall,double raininess,int nodeid);
        int selectMySQL_voltagePrevious(QString dayPrevious,double &voltage,int nodeid);
};
#endif // DBSERVER_H
