#include "threadsocketrecv.h"


threadSocketRecv::threadSocketRecv(int handler, QObject *parent):

    QThread(parent)

{
    this->handler=handler;
    dataRecv.clear();
    wholePac.clear();
    status=leisure;
//    pStatus->headIndex=0;
//    pStatus->length=0;
//    pStatus->tailIndex=0;
//    pStatus->timeSlice=false;
//    pStatus->status=leisure;
//    pStatus->wholeData.clear();

}

void threadSocketRecv::run()

{

    socket=new QTcpSocket();

    if(!socket->setSocketDescriptor(handler))

    {
        qDebug()<<"errorConnect";
        qDebug()<<socket->errorString();

        emit error(socket->error());

    }
    else
    {
        qDebug()<<"Connected ";

    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);

    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    exec();

}

void threadSocketRecv::readyRead()

{
    //read all data from the socket
//    dataRecv=socket->read(2);
//    //if not empty, push it into a queue
//    if(!dataRecv.isEmpty())
      dataList.enqueue(socket->read(2));

//    while(!dataList.isEmpty())
//    {
//       dataRecv=dataList.dequeue();
//        if(dataRecv.toHex()=="7e45")
//        {
//           qDebug()<<"Find the head";
//        }
//        else if(dataRecv.toHex().mid(2,-1)=="7e")
//        {
//            qDebug()<<"Find it,2";

//         }
//        else
//             qDebug()<<"Searching...";
////        QByteArray t=dataList.dequeue();
////        qDebug()<<t.toHex().data();

//   }



        while(!dataList.isEmpty())
        {
            dataRecv=dataList.dequeue();
            //find the head,Flag it,change the status of the packet
            if(dataRecv.toHex()=="7e45")
            {
                status=find_tail;//find the tail
            }
            //find the tail,Flag it
            else if(dataRecv.toHex().mid(2,-1)=="7e")
            {
                status=find_head;
                wholePac+=dataRecv;
            }
            switch(status)
            {
            case find_tail:
                wholePac+=dataRecv;
                break;
            case find_head:
                status=leisure;
                qDebug()<<wholePac.toHex().data();
                wholePac.clear();
                break;
            default:
                qDebug()<<"Searching...";
            }

        }


}

void threadSocketRecv::disconnected()
{


    socket->deleteLater();
    exit(0);
}
int  threadSocketRecv::switchNum(char ch)//十六进制转换成十进制
{
    if(ch>='0'&&ch<='9')
        return ch-'0';
    else if(ch=='a'||ch=='A')
        return 10;
    else if(ch=='b'||ch=='B')
        return 11;
    else if(ch=='c'||ch=='C')
        return 12;
    else if(ch=='d'||ch=='D')
        return 13;
    else if(ch=='e'||ch=='E')
        return 14;
    else
        return 15;
}
