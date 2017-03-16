#include "server.h"
#include "thread.h"
server::server(QObject *parent) :
    QTcpServer(parent)
{

}

void server::connectHost()
{
    QString port="7900";

    if(!this->listen(QHostAddress::Any,port.toUInt()))//无法连接
    {
        QFile file("connectException.txt");
        if(file.open(QIODevice::WriteOnly|QIODevice::WriteOnly|QIODevice::Append))
        {
           QTextStream fileOut(&file);
           fileOut<<"   "+this->errorString()<<"\r\n";
           file.close();
        }
    }
    else
        qDebug()<<"OK33006";

}

void server::incomingConnection(int handle)
{
    qDebug() << " Connecting...";
    threadSocketRecv* thread=new threadSocketRecv(handle,this);

    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
    if(thread->isRunning())
    {
         qDebug() << handle<<"thread is running";
    }
    else
       qDebug() << " wrong";

}
