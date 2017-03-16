#ifndef IOTHREAD_H
#define IOTHREAD_H

#include <QThread>
class TcpSocket;
class iothread : public QThread
{
    Q_OBJECT
public:
    explicit iothread(QObject *parent = 0);
    void connectHost();
    void disconnectHost();
signals:
    
public slots:
protected:
    void run();
private:
    TcpSocket* socket;
};

#endif // IOTHREAD_H
