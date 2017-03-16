#ifndef SOCKET_H
#define SOCKET_H
#include<QTcpSocket>

class socket:public QTcpSocket
{
    Q_OBJECT
public:
    explicit socket(QObject *parent = 0);
};

#endif // SOCKET_H
