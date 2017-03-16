#ifndef THREADDATAPROCESS_H
#define THREADDATAPROCESS_H
#include<QThread>
class threadDataProcess: public QThread
{
    Q_OBJECT
public:
    explicit threadDataProcess(int handler, QObject *parent = 0);
private:
    int handler;
};

#endif // THREADDATAPROCESS_H
