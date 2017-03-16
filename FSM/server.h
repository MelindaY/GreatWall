#ifndef SERVER_H
#define SERVER_H
#include <QTcpSocket>
#include<QTcpServer>
#include<QFile>
#include<threadsocketrecv.h>
class server:public QTcpServer
{
     Q_OBJECT
public:
    explicit server(QObject *parent = 0);
    void connectHost();
protected:
    void incomingConnection(int handle);


};

#endif // SERVER_H
