#include "thread.h"

thread::thread(int handler, QObject *parent):
    QThread(parent)
{
    this->handler=handler;
}
void thread::run()
{
    socket=new QTcpSocket();
    if(!socket->setSocketDescriptor(handler))
    {
        emit error(socket->errorString());
    }
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
}
void thread::readyRead()
{
    socket->readData(recvData,sizeof(recvData));
}
