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
    int handler; //�߳̾��
    QByteArray dataRecv;//��������
    QQueue < QByteArray> dataList;//�������ݶ���
    QByteArray wholePac;
    int status; //����״̬��find_head:�ҵ���ͷ find_tail:�ҵ���β leisure:����
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
