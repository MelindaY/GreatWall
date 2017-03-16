#ifndef MYSERVER_H
#define MYSERVER_H
#include<QUdpSocket.h>
#include<config.h>
#include<dbserver.h>
#include<tool.h>
class myServer : public QUdpSocket
{
    Q_OBJECT
public:
    explicit myServer(QObject *parent = 0);
    void createDataBase(Config* config);//ʹ��config�������ݿ�
    void setConfig(Config* config);//��ʼ�������е�config
    void translateData(QByteArray data); //��������
private:
    QUdpSocket *udpSocket;
    Config *config;//�����ļ�
    DBServer *dataBase;
    QByteArray controlPac;//���ư�
    QByteArray dataPac;//���ݰ�

    QByteArray buf;//�������ݵĻ�����
    int  count_control;//���ư��ķ�����
    int count_data;//���ݰ��ķ�����
    int totalFlow;//������
    int storageNum;//�����
    int exceptNum;//�쳣��
    QString storageTime_v1;//������ʱ��
    QString storageTime_v2;//������ʱ��
    int value;//�жϰ�����

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
    double tem;//
    int countControl;

    int stateFlag;//�ڵ��봫����ͨ��״̬��־��0--������1--У�����2--ͨ���ж�
    //������������ʪ�ȴ������ڵ�
    int addressCode;//��ַ��
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
    int rain;//�Ƿ���
    //�������ηִ������ڵ�
    int saltaddrcode;//��ַ��
    double verticalSalt;
    double surfaceSalt;

    bool flag;

    //�𶯴���������
    double shake1;
    double shake2;
    double shake3;

    QString exceptTime;

private slots:
    void readPendingDatagrams();
signals:
    void updateData(QString data);
};

#endif // MYSERVER_H
