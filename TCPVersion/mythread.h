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
    explicit MyThread(QObject *parent = 0);//构造函数
    ~MyThread();//析构函数
    void setMyThreadID(int id);//设置线程ID
    void setgateway();//设置网关ID
    void setHandle(int handle);
    //将MyServer的handle传递给socket的handle
    void setDBServer(DBServer* dataBase);
    void setDBSeverBackup(DBServer* dataBase);
    //将MyServer的dataBase传递给MyThread的dataBase
    void setTimeStamp();
signals:
    void linkRup();//连接中断
    void dataInfo(int id,QString gateway,int totalFlow,
                  int storageNum,int exceptNum,
                  QString storageTime,int nodeID,
                  QString arriveTime);
    void updatetimeStamp(QString timeStamp_v1,QString storageTime_v2);
    //将接收的数据显示
private slots:
    void recvString();//接收数据
private:
    int id;//线程编号
    //    int nodeID;       //节点ID  37字节
    //    QString arriveTime;//接入时间
    //    int dataFlag;
    //    double dataValue; //数据信息
    //    double  voltage;
    //    double  temperature;        //温度
    QTcpSocket *socket;

    QByteArray buffer;//接收数据的缓冲区
    QByteArray controlPac;//控制包
    QByteArray dataPac;//数据包

    DBServer* dataBase;//数据库
    DBServer* dataBaseBackup;//数据库
    QString gateway;//网关
    int  count_control;//控制包的发包数
    int count_data;//数据包的发包数
    int totalFlow;//总流量
    int storageNum;//入库数
    int exceptNum;//异常数
    QString storageTime_v1;//最近入库时间
    QString storageTime_v2;//最近入库时间
    double RSSI;
    double coorVoltage;

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
//    double tem;//
    int countControl;

    int stateFlag;//节点与传感器通信状态标志，0--正常，1--校验错误，2--通信中断
    //以下是土壤温湿度传感器节点
    int addressCode1;//地址码
    int addressCode2;//地址码
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
    double voltagePre; //上一次电压值
    int rain;//是否降雨
    int rainDate; //降雨的第几天
    int emptyTimes;//用了几次第二个雨量公式
    QDateTime rainTime;
    bool timeFlag;//保证第二条12点数据不用进行计算
    QString DayPrevious;//前一天的日期
    double PreviousVoltage;//前一天的电压
    double dailyRain;//日降雨量

    //以下是盐分传感器节点
    int saltaddrcode;//地址码
    double tem;
//    double hum;
    double verticalSalt;
    double surfaceSalt;
    bool flag;

    //震动传感器数据
    double shake1;
    double shake2;
    double shake3;

    int sizeFlag;  //判断数据包是新数据包还是旧的 true：新数据包（52）

    QString timeStamp;//网关时间戳实现
    QString timeStamp_v1;
    QDateTime standardtime;
    QString timeFinished;
    int timeCount;//time count to reflush time

    double parameters[150][14]; //土温参数设定
    double saltpara[109][2];
        int count;

};

#endif // MYTHREAD_H
