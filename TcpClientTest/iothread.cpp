#include "iothread.h"
#include "tcpsocket.h"
#include <QHostAddress>
iothread::iothread(QObject *parent) :
    QThread(parent)
{
    this->socket=new TcpSocket;
}

void iothread::connectHost()
{
    socket->connectToHost(QHostAddress("127.0.0.1"),7900);
}

void iothread::disconnectHost()
{
    socket->disconnectFromHost();
}

void iothread::run()
{
    char data1[26]={0x7E,0x45,0x03,0x09,0x9b,0x74,0xc0,0xc3,0x7f,0xfc,0x30,0x01,0x6f,0x00,0x00,0x00,0x37,0x11,0x00,0x00,0x00,0x5b,0xcf,0x00,0x00,0x7E};
    char data2[26]={0x7E,0x45,0x0e,0x35,0x03,0x02,0x00,0x19,0x0f,0x01,0x80,0x30,0x0a,0x0f,0x01,0x73,0x34,0xe3,0x0f,0x01,0x60,0x22,0xc9,0x00,0x00,0x7E};
    char data4[26]={0x7E,0x45,0x0e,0x65,0x01,0x02,0x03,0x0d,0x1f,0x13,0x0c,0x04,0x24,0x01,0x07,0x19,0x44,0x00,0x45,0x0b,0x54,0x00,0x00,0x00,0x00,0x7E};
    char data5[26]={0x7E,0x45,0x0e,0x1b,0x02,0x03,0x00,0x17,0x0f,0x01,0x56,0x33,0x23,0x0f,0x01,0x41,0x2a,0x41,0x0f,0x01,0x21,0x21,0xb5,0x00,0x00,0x7E};
    char data6[26]={0x7E,0x45,0x00,0x10,0x02,0x02,0x00,0x2d,0x0f,0x01,0x53,0x21,0xdf,0x0f,0x01,0x43,0x25,0xaf,0x0f,0x01,0x59,0x27,0xba,0x00,0x00,0x7E};
    char data0[26]={0x7E,0x45,0x00,0x00,0x02,0x04,0x04,0x01,0x2D,0x0D,0x23,0x01,0x90,0x41,0x01,0x4C,0x02,0xC5,0x1A,0xD8,0x05,0x88,0x90,0x54,0xF3,0x7E};
    char data7[26]={0x7E,0x45,0x00,0x00,0x0D,0x00,0x01,0x00,0x41,0x01,0x4C,0x02,0xC5,0x1A,0xD8,0x05,0x88,0x00,0x54,0xE9,0x52,0xF3,0x7E};

//    char data1[12]={0x7E,0x45,0x01,0x09,0x10,0x00,0x37,0x11,0x00,0x5b,0xcf,0x7E};
//    char data2[12]={0x7E,0x45,0x02,0x09,0x10,0x00,0x37,0x11,0x00,0x5b,0xcf,0x7E};
//    char data3[12]={0x7E,0x45,0x02,0x09,0x10,0x00,0x37,0x11,0x00,0x5b,0xcf,0x7E};
//    char data4[12]={0x7E,0x45,0x02,0x09,0x10,0x00,0x37,0x11,0x00,0x5b,0xcf,0x7E};
//    char data5[12]={0x7E,0x45,0x02,0x09,0x10,0x00,0x37,0x11,0x00,0x5b,0xcf,0x7E};
//    char data6[12]={0x7E,0x45,0x02,0x09,0x10,0x00,0x37,0x11,0x00,0x5b,0xcf,0x7E};
//    char data7[26]={0x7E,0x45,0x00,0xf9,0x02,0x04,0x80,0x27,0x0f,0x01,0x76,0x01,0x7a,0x00,0x00,0x00,0x3c,0xa6,0x00,0x00,0x00,0x50,0xc4,0x00,0x00,0x7E};
    int i=0,j=1;
    while(true)
    {
        if(j==60)
        {
            socket->write(data7,sizeof(data7));
            j=0;
        }
        else if(i==1)
        {
            socket->write(data1,sizeof(data1));
        }
        else if(i==2)
        {
            socket->write(data2,sizeof(data2));
        }
        else if(i==3)
        {
            socket->write(data4,sizeof(data4));
        }
        else if(i==4)
        {
            socket->write(data5,sizeof(data5));
        }
        else if(i==5)
        {
            socket->write(data6,sizeof(data6));
        }
//        else if(i==0)
//        {
//            socket->write(data0,sizeof(data0));
//        }
        socket->flush();
        sleep(1);
        i=(i+1)%6;
        j++;

    }
}
