#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QTcpSocket>
#include <QByteArray>
#include<QDateTime>
#include<QDebug>
#define SIZE 1024
class DBServer;

class MyThread : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = 0);//���캯��
    ~MyThread();//��������
    void setMyThreadID(int id);//�����߳�ID
    void setgateway();//��������ID
    void setHandle(int handle);
    //��MyServer��handle���ݸ�socket��handle
    void setDBServer(DBServer* dataBase);
    void setDBSeverBackup(DBServer* dataBase);
    //��MyServer��dataBase���ݸ�MyThread��dataBase
    void setTimeStamp();
signals:
    void linkRup();//�����ж�
    void dataInfo(int id,QString gateway,int totalFlow,
                  int storageNum,int exceptNum,
                  QString storageTime,int nodeID,
                  QString arriveTime);
    void updatetimeStamp(QString timeStamp_v1,QString storageTime_v2);
    //�����յ�������ʾ
private slots:
    void recvString();//��������
private:
    int id;//�̱߳��
    //    int nodeID;       //�ڵ�ID  37�ֽ�
    //    QString arriveTime;//����ʱ��
    //    int dataFlag;
    //    double dataValue; //������Ϣ
    //    double  voltage;
    //    double  temperature;        //�¶�
    QTcpSocket *socket;

    QByteArray buffer;//�������ݵĻ�����
    QByteArray controlPac;//���ư�
    QByteArray dataPac;//���ݰ�

    DBServer* dataBase;//���ݿ�
    DBServer* dataBaseBackup;//���ݿ�
    QString gateway;//����
    int  count_control;//���ư��ķ�����
    int count_data;//���ݰ��ķ�����
    int totalFlow;//������
    int storageNum;//�����
    int exceptNum;//�쳣��
    QString storageTime_v1;//������ʱ��
    QString storageTime_v2;//������ʱ��
    double RSSI;
    double coorVoltage;

    int hop[5];//����·��
    int hopnum;//��������
    int PacketNum;//�ѷ������ݰ�����
    int nodetype;//�ڵ����ͣ���ʪ�ȹ��սڵ㣿������ʪ�Ƚڵ㣿�η֣��ȣ�

    int nodeID;       //�ڵ�ID
    QString arriveTime;//����ʱ��
    //��������ʪ�ȹ��մ������ڵ�
    double fVoltage; //��ѹ
    double fLight;   //����
    double fTemperature; //�¶�
    double fHumidity; //ʪ��
    double fCrackdata;//�ѷ�


    double hum;//������ʪ��
//    double tem;//
    int countControl;

    int stateFlag;//�ڵ��봫����ͨ��״̬��־��0--������1--У�����2--ͨ���ж�
    //������������ʪ�ȴ������ڵ�
    int addressCode1;//��ַ��
    int addressCode2;//��ַ��
    double tem_30cm;//������30cm���¶�ֵ
    double hum_30cm;//������30cm��ʪ��ֵ
    double tem_15cm;
    double hum_15cm;
    double tem_5cm;
    double hum_5cm;
    //���������������괫�����ڵ�
    int rainnode_addr;
    double tem_outside;//�ⲿ�¶�
    double tem_inside;//�ڲ��¶�
    double ph;
    double rainfall;//������
    double windspeed;//����
    double winddirection;//����
    double powervoltage;//��Դ��ѹ
    double voltagePre; //��һ�ε�ѹֵ
    int rain;//�Ƿ���
    int rainDate; //����ĵڼ���
    int emptyTimes;//���˼��εڶ���������ʽ
    QDateTime rainTime;
    bool timeFlag;//��֤�ڶ���12�����ݲ��ý��м���
    QString DayPrevious;//ǰһ�������
    double PreviousVoltage;//ǰһ��ĵ�ѹ
    double dailyRain;//�ս�����

    //�������ηִ������ڵ�
    int saltaddrcode;//��ַ��
    double tem;
//    double hum;
    double verticalSalt;
    double surfaceSalt;
    bool flag;

    //�𶯴���������
    double shake1;
    double shake2;
    double shake3;

    int sizeFlag;  //�ж����ݰ��������ݰ����Ǿɵ� true�������ݰ���52��

    QString timeStamp;//����ʱ���ʵ��
    QString timeStamp_v1;
    QDateTime standardtime;
    QString timeFinished;
    int timeCount;//time count to reflush time

    double parameters[150][14]; //���²����趨
    double saltpara[109][2];
        int count;

};

#endif // MYTHREAD_H
