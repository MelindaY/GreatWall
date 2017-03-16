#ifndef CUTDATA_H
#define CUTDATA_H
#include <QThread>
#include<QTcpSocket>
#include"socket.h"
class socket;
class cutData:public QThread
{
   Q_OBJECT
public:
     explicit cutData(QObject *parent = 0);
//protected:
//     void run();
private:
     socket *socketRead;
};

#endif // CUTDATA_H
