#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class MyServer;
class Config;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);//���캯��
    ~MainWindow();//��������
public slots:
    void recvupdateLinkNum(int linkNum);
    //���½����LinkNumֵ
    void updateDataInfo(int id,QString gateway,int totalFlow,
                        int storageNum,int exceptNum,
                        QString storageTime,int nodeID,
                        QString arriveTime);
    //���½�����ʾ��ֵ
    void recvNewClientComing();
    //�������ӵ���ʱ������Realtime Status����ı������
    void recvRuningState(QString info);//�ڽ�������ʾ������Ϣ
    void recvStateInfo(QString state);//�ڽ�������ʾ����״̬��Ϣ
    void qtimeSlot();//ʱ�����
    void recvTimeStamp(QString stampTime,QString currentTime);

private slots:
    void on_ExitBotton_2_clicked();//�������˳���ť��ʵ��

private:
    Ui::MainWindow *ui;//�������
    MyServer *server; //���̶߳���,���ڵȴ����صĵ���
    Config *config;//�����ļ�����������ʹ�������ļ�����Ҫ�ǻ�ȡ���ն˿�
    QTimer *timer;//��ʱ��
};

#endif // MAINWINDOW_H
