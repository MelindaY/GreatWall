#ifndef THREADSOCKETRECV_H
#define THREADSOCKETRECV_H

#define HEAD "7e45"
#define TAIL "7e00"
#define LENGTH 26
#define find_head 1
#define find_tail 2
#define leisure 0
#include<QThread>
#include <QTcpSocket>
#include<QQueue>

class threadSocketRecv : public QThread
{

    Q_OBJECT
public:

    explicit threadSocketRecv(int handler, QObject *parent = 0);

    void run();
    int  switchNum(char ch);//HEX2DECIMAL
signals:

    void error(QTcpSocket::SocketError socketerror);


public slots:

    void readyRead();

    void disconnected();

private :

    QTcpSocket *socket;
    int handler; //线程句柄
    QByteArray dataRecv;//接收数据
    QQueue < QByteArray> dataList;//接收数据队列
    QByteArray wholePac;
    int status; //包的状态：find_head:找到包头 find_tail:找到包尾 leisure:空闲
    char pacCut[2];
//    struct Packet
//    {
//     int headIndex;
//     bool headFlag;
//     int tailIndex;
//     bool tailFlag;
//     int length;
//     bool timeSlice;
//     QByteArray Data;
//     QByteArray wholeData;
//     int status;
//    } *pStatus;

};




#endif // THREADSOCKETRECV_H
