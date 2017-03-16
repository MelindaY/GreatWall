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
    void createDataBase(Config* config);//使用config连接数据库
    void setConfig(Config* config);//初始化此类中的config
    void translateData(QByteArray data); //解析数据
private:
    QUdpSocket *udpSocket;
    Config *config;//配置文件
    DBServer *dataBase;
    QByteArray controlPac;//控制包
    QByteArray dataPac;//数据包

    QByteArray buf;//接收数据的缓冲区
    int  count_control;//控制包的发包数
    int count_data;//数据包的发包数
    int totalFlow;//总流量
    int storageNum;//入库数
    int exceptNum;//异常数
    QString storageTime_v1;//最近入库时间
    QString storageTime_v2;//最近入库时间
    int value;//判断包类型

    int hop[5];//跳步路径
    int hopnum;//跳步数量
    int PacketNum;//已发送数据包总量
    int nodetype;//节点类型（温湿度光照节点？土壤温湿度节点？盐分？等）

    int nodeID;       //节点ID
    QString arriveTime;//接入时间
    //以下是温湿度光照传感器节点
    double fVoltage; //电压
    double fLight;   //光照
    double fTemperature; //温度
    double fHumidity; //湿度
    double fCrackdata;//裂缝


    double hum;//空气温湿度
    double tem;//
    int countControl;

    int stateFlag;//节点与传感器通信状态标志，0--正常，1--校验错误，2--通信中断
    //以下是土壤温湿度传感器节点
    int addressCode;//地址码
    double tem_30cm;//土壤深30cm的温度值
    double hum_30cm;//土壤深30cm的湿度值
    double tem_15cm;
    double hum_15cm;
    double tem_5cm;
    double hum_5cm;
    //以下是雨量、酸雨传感器节点
    int rainnode_addr;
    double tem_outside;//外部温度
    double tem_inside;//内部温度
    double ph;
    double rainfall;//降雨量
    double windspeed;//风速
    double winddirection;//风向
    double powervoltage;//电源电压
    int rain;//是否降雨
    //以下是盐分传感器节点
    int saltaddrcode;//地址码
    double verticalSalt;
    double surfaceSalt;

    bool flag;

    //震动传感器数据
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
