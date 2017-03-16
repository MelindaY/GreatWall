#ifndef THREAD_H
#define THREAD_H
#include<QThread>
#include <QTcpSocket>
#include<QQueue.h>
class thread : public QThread
{
    Q_OBJECT
public:
    explicit thread(int handler, QObject *parent = 0);
    void run();
signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();
private :
    QTcpSocket *socket;
    int handler;
    QQueue<char*> dataRecv;
};

#endif // THREAD_H
